#pragma once
#include <stdio.h>
#include <stdlib.h>


#include "raylib.h"
#include "da.h"



typedef struct {
    Rectangle rect;
    Vector2 speed; 
    Vector2 max;
    Vector2 min;
    double bumper;
    bool kill; 
    bool move;
    bool end;
    bool start;
} Platform;

         
typedef struct {
    Platform *items;
    size_t  count;
    size_t  capacity;
} Platforms;



Platforms get_level(int level_id);
void unload_level(Platforms *p);
