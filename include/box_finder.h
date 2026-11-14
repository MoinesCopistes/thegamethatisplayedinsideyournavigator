#pragma once
#include <stddef.h>
#include "raylib.h"

typedef struct {
    Rectangle rect;
    Texture2D texture;
    Vector2 pos;
} Region;


typedef struct {
    Region *items;
    size_t  count;
    size_t  capacity;

    Texture2D debug_image;
} DetectionResult;

DetectionResult process_screen(const char *path);
void            unload_detection_result(DetectionResult *r);

