#pragma once
#include <stddef.h>

#include "stdbool.h"
#include "box_finder.h"
#include "level_manager.h"





void remove_texture_region(Image *img, DetectionResult *r);


void get_transformations(Platforms *p, DetectionResult *r, bool *usage_mask);

Vector2 update_rec_position(int frame, int max_frames, int index, DetectionResult *res); 
