#pragma once
#include <stdio.h>
#include <stdlib.h>


#include "raylib.h"
#include "da.h"



typedef struct {
    Rectangle rect;
    bool kill; 
    double bumper; // bounce level
    bool move;
    Vector2 speed; 
    Vector2 max;
    Vector2 min;
} Platform;

         
typedef struct {
    Platform *items;
    size_t  count;
    size_t  capacity;
} Platforms;



Platforms get_level();
void unload_level(Platforms *p);
