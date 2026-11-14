#include "level_assembler.h"
#include "box_finder.h"
#include "reasings.h"

#define TRESHOLD_PX 10


void remove_texture_region(Image *img, DetectionResult *r) {
    for (size_t i = 0; i < r->count; i++) {
        ImageDrawRectangleRec(img, r->items[i].rect, WHITE);
    }
}

void get_transformations(Platforms *p, DetectionResult *r, bool *usage_mask) {
    for (size_t i = 0; i < r->count; i++) {
        usage_mask[i] = 0;
    }

    for (size_t i = 0; i < p->count; i++) {
        Rectangle platform_rect = p->items[i].rect;
        int bordertop = platform_rect.width;
        int borderbottom = platform_rect.width;
        int borderright = platform_rect.height;
        int borderleft = platform_rect.height;

        for (size_t j = 0; j < r->count; j++) {
            if (usage_mask[j]) continue;
            Rectangle region_rect = r->items[j].rect;

            // detection is bigger than half the platform, reject to avoid overlapping
            if ((region_rect.width >= (int) platform_rect.width / 2) || (region_rect.height >= (int) platform_rect.height)) continue;

            if (bordertop > region_rect.width) {
                r->items[j].final_pos = (Vector2) {.x = platform_rect.x + platform_rect.width - bordertop, .y = platform_rect.y};
                bordertop = bordertop - region_rect.width;
                // top left corner
                if (bordertop == platform_rect.width && borderleft == platform_rect.height) {
                    borderleft = borderleft - region_rect.height;
                }
                usage_mask[j] = true;
                continue;
            }
            if (borderleft > region_rect.height) {
                r->items[j].final_pos = (Vector2) {.x = platform_rect.x, .y = platform_rect.y + platform_rect.height - borderleft};
                borderleft = borderleft - region_rect.height;
                // bottom left corner
                if (borderleft < TRESHOLD_PX) {
                    borderbottom = borderbottom - region_rect.width;
                }
                usage_mask[j] = true;
                continue;
            }
            if (borderbottom > region_rect.width) {
                r->items[j].final_pos = (Vector2) {.x = platform_rect.x + platform_rect.width - borderbottom, .y = platform_rect.y + platform_rect.height - region_rect.height};
                borderbottom = borderbottom - region_rect.width;
                usage_mask[j] = true;
                continue;
            }
            if (borderright > region_rect.height) {
                r->items[j].final_pos = (Vector2) {.x = platform_rect.x + platform_rect.width - region_rect.width, .y = platform_rect.y + platform_rect.height - borderright},
                borderright = borderright - region_rect.height;
                usage_mask[j] = true;
                continue;
            }
        }
    }
}
Vector2 update_rec_position(int frame, int max_frames, int index, DetectionResult *res) {
    Region reg = res->items[index];
    
    float change_x = reg.final_pos.x - reg.pos.x;
    float change_y = reg.final_pos.y - reg.pos.y;

    int pos_x = EaseCubicInOut(frame, reg.pos.x, change_x, max_frames);
    int pos_y = EaseCubicInOut(frame, reg.pos.y, change_y, max_frames);
    
    return (Vector2){
        .x = pos_x, .y = pos_y
    };
}
