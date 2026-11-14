#include "bike.h"
#include "level_manager.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void step_verlet(VerletVector *vector, Vector *A, double throttle, double dt) {
  double old_x = vector->x;
  double old_y = vector->y;
  vector->x = 2 * vector->x - vector->prev_x + (A->x + throttle) * dt * dt;
  vector->y = 2 * vector->y - vector->prev_y + A->y * dt * dt;
  vector->prev_x = old_x;
  vector->prev_y = old_y;
}
void add_force(Bike *bike, Vector force) {
  bike->acc1.x += force.x;
  bike->acc1.y += force.y;
  bike->acc2.x += force.x;
  bike->acc2.y += force.y;
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

      if (CheckCollisionCircleRec((Vector2){(*wheels[j]).x, (*wheels[j]).y},
                                  bike->radius, p.items[i].rect) &&
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

void slow_down_wheel(VerletVector *v, double friction, double dt) {
  double damping = exp(-friction * dt);

  v->prev_x = v->x - (v->x - v->prev_x) * damping;
}

void slow_down(Bike *bike, double dt) {
  slow_down_wheel(&bike->w1, 1.5, dt);
  slow_down_wheel(&bike->w2, 1.5, dt);
}

void step(Bike *bike, double dt, Platforms p) {

  bike->on_ground = 0;
  slow_down(bike, dt);
  step_verlet(&bike->w1, &bike->acc1, bike->throttle, dt);
  step_verlet(&bike->w2, &bike->acc2, 0, dt); // we accelerate via back wheel

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
  add_force(b, (Vector){.x = 0, y = GRAVITY});
  return b;
}

