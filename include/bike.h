#ifndef bike_h_INCLUDED
#define bike_h_INCLUDED

#include "level_manager.h"
#include <stdint.h>

#define BIKE_LENGTH 120
#define GRAVITY 300

typedef struct {
  double x;
  double y;
  double prev_x;
  double prev_y;
} VerletVector;

typedef struct {
  double x;
  double y;
} Vector;

typedef struct {
  VerletVector w1; // wheel 1 x
  VerletVector w2;
  Vector acc1;
  Vector acc2;
  uint32_t radius;
  double throttle;
  bool on_ground;
} Bike;

void step(Bike *bike, double dt, Platforms p);

Bike *create_bike(double x, double y, uint32_t radius);

void add_force(Bike *bike, Vector force);
#endif // bike_h_INCLUDED
