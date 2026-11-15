#ifndef bike_h_INCLUDED
#define bike_h_INCLUDED

#include "level_manager.h"
#include "raylib.h"
#include <stdint.h>

#define BIKE_LENGTH 50
#define GRAVITY 1200
#define THROTTLE_ACCEL 1700
#define TORQUE_ACCEL 130
#define JUMP_SPEED 400

typedef struct {
  double x;
  double y;
  double prev_x;
  double prev_y;
} VerletVector;


typedef struct {
  VerletVector w1; // wheel 1 x
  VerletVector w2;
  Vector2 acc1;
  Vector2 acc2;
  uint32_t radius;
  double throttle;
  double torque;
  bool on_ground;
} Bike;

void step(Bike *bike, double dt, Platforms p);

Bike *create_bike(double x, double y, uint32_t radius);

void jump(Bike* bike, double dt);

#endif // bike_h_INCLUDED
