#include "bike.h"
#include "level_manager.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void step_verlet(VerletVector *vector, Vector2 *A, double dt) {
    double old_x = vector->x;
    double old_y = vector->y;
    vector->x = 2 * vector->x - vector->prev_x + A->x * dt * dt;
    vector->y = 2 * vector->y - vector->prev_y + A->y * dt * dt;
    vector->prev_x = old_x;
    vector->prev_y = old_y;
}

void constraint_wheels(Bike *bike) {

    double dx = bike->w2.x - bike->w1.x;
    double dy = bike->w2.y - bike->w1.y;

    double dist = sqrt(dx * dx + dy * dy);
    if (dist < 1e-9)
        return;

    double correction = (dist - BIKE_LENGTH) / dist;
    bike->w1.x += dx * correction * 0.5;
    bike->w1.y += dy * correction * 0.5;

    bike->w2.x -= dx * correction * 0.5;
    bike->w2.y -= dy * correction * 0.5;
}
void constraint_platforms(Bike *bike, Platforms p) {
    for (int i = 0; i < p.count; i++) {
        VerletVector *wheels[] = {&bike->w1, &bike->w2};
        for (int j = 0; j < sizeof(wheels) / sizeof(wheels[0]); j++) {

            if (CheckCollisionCircleRec(
                    (Vector2){(*wheels[j]).x, (*wheels[j]).y}, bike->radius,
                    p.items[i].rect) &&
                (wheels[j])->prev_y + bike->radius <= p.items[i].rect.y) {
                (*wheels[j]).y = p.items[i].rect.y - bike->radius;

                bike->on_ground = 1;
            }
        }
    }
}

void constraint(Bike *bike, Platforms p) {
    // TODO: Add real collision
    for (int i = 0; i < 10; i++) {
        constraint_wheels(bike);
        constraint_platforms(bike, p);
    }
}

void slow_down_wheel(Bike *bike, VerletVector *v, double friction, double dt) {
    double f =
        bike->on_ground ? 2.0 : 1.0; // strong rolling friction, tiny air drag
    double damping = exp(-f * dt);
    v->prev_x = v->x - (v->x - v->prev_x) * damping;
    v->prev_y = v->y - (v->y - v->prev_y) * damping;
}
void slow_down(Bike *bike, double dt) {
    slow_down_wheel(bike, &bike->w1, 1.5, dt);
    slow_down_wheel(bike, &bike->w2, 1.5, dt);
}

void apply_throttle(Bike *bike, double dt) {
    if (!bike->on_ground || bike->throttle == 0)
        return;
    bike->acc2.x += bike->throttle * THROTTLE_ACCEL;
}
void apply_torque(Bike *bike, double dt) {
    // bike axis
    double ax = (bike->w2.x - bike->w1.x) / BIKE_LENGTH;
    double ay = (bike->w2.y - bike->w1.y) / BIKE_LENGTH;
    // perpendicular
    double px = -ay, py = ax;

    bike->acc1.x += bike->torque * TORQUE_ACCEL * px;
    bike->acc1.y += bike->torque * TORQUE_ACCEL * py;
    bike->acc2.x -= bike->torque * TORQUE_ACCEL * px;
    bike->acc2.y -= bike->torque * TORQUE_ACCEL * py;
}

void jump(Bike *bike, double dt) {
    if (!bike->on_ground)
        return;
    bike->w2.prev_y += JUMP_SPEED * 1.09 * dt;
    bike->w1.prev_y += JUMP_SPEED * dt;
}

void step(Bike *bike, double dt, Platforms p) {
    bike->acc1 = (Vector2){0, GRAVITY};
    bike->acc2 = (Vector2){0, GRAVITY};
    apply_throttle(bike, dt);
    slow_down(bike, dt);
    apply_torque(bike, dt);
    step_verlet(&bike->w1, &bike->acc1, dt);
    step_verlet(&bike->w2, &bike->acc2, dt); // we accelerate via back wheel

    bike->on_ground = 0;
    constraint(bike, p);
}

Bike *create_bike(double x, double y, uint32_t radius) {
    Bike *b = calloc(sizeof(Bike), 1);
    b->radius = radius;
    b->w1.x = x;
    b->w1.y = y;
    b->w1.prev_x = x;
    b->w1.prev_y = y;
    b->w2.x = x + BIKE_LENGTH;
    b->w2.y = y;
    b->w2.prev_x = x + BIKE_LENGTH;
    b->w2.prev_y = y;
    b->on_ground = 0;
    return b;
}

