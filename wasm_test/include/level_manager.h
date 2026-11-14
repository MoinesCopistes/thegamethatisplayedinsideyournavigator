#pragma once
#include <stdio.h>
#include <stdlib.h>


#include "raylib.h"
#include "da.h"



typedef struct {
    Rectangle rec;
    bool kill; 
} Platform;

         
typedef struct {
    Rectangle rect;
    bool danger;
    Platform *items;
    size_t  count;
    size_t  capacity;
} Platforms;



Platforms get_level();
void unload_level(Platforms *p);
