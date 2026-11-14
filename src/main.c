#include "raylib.h"
#include "box_finder.h"
#include "level_manager.h"
#include "level_assembler.h"

int main() {
    TraceLog(LOG_INFO, "starting mouhahahha");


    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    // SetConfigFlags(FLAG_FULLSCREEN_MODE);

    InitWindow(1531,959, "mouhahah");


    DetectionResult res = process_screen("tests/wiki.png");
    TraceLog(LOG_INFO, "tex id=%u size=%dx%d regions=%zu",
         res.debug_image.id,
         res.debug_image.width, res.debug_image.height,
         res.count);
    Platforms plat = get_level();
    bool usage_mask[res.count];
    for (size_t i = 0; i < res.count; i++) {
        usage_mask[i] = 0;
    }
    get_transformations(&plat, &res, usage_mask);
    bool animation_finished = false;
    size_t last_anim_idx = -1;
    for (size_t i = 0; i < res.count; i++) {
        if (usage_mask[i]) last_anim_idx = i;
    }

    Image background_img = LoadImage("tests/wiki.png");
    // remove_texture_region(&background_img, &res);
    Texture2D background = LoadTextureFromImage(background_img);
    UnloadImage(background_img);


    int animation_frame_count = 0;
    int fading_frame_count = 0;
    
    SetTargetFPS(60);
    TraceLog(LOG_INFO, "Init window");

    while (!WindowShouldClose()) {
        animation_frame_count++;
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (animation_finished && fading_frame_count < 60) {
            fading_frame_count++;
            float fade = 1.0f - ((float)fading_frame_count / 60.0f);
            DrawTexture(background, 0, 0, Fade(WHITE, fade));
        } else if (fading_frame_count < 60) {
            DrawTexture(background, 0, 0, WHITE);
        }


        int stagger_delay = 4;
        int anim_duration = 60;
        int active_index = 0;
        for (size_t i = 0; i < res.count; i++) {
            if (!usage_mask[i]) continue;

            int start_frame = active_index * stagger_delay;
            int local_frame = animation_frame_count - start_frame;
            active_index++;
            Region reg = res.items[i];
            if (local_frame <= 0) {
                DrawTextureV(reg.texture, reg.pos, WHITE);
            }
            else if (local_frame < anim_duration) {
                Vector2 pos = update_rec_position(local_frame, anim_duration, i, &res);
                DrawTextureV(reg.texture, pos, WHITE);
            }
            else {
                DrawTextureV(reg.texture, reg.final_pos, WHITE);
                if (i == last_anim_idx) {
                    animation_finished = true;
                }
            }
        }
       
        EndDrawing();
    }
    UnloadTexture(background);
    unload_level(&plat);
    unload_detection_result(&res);
    CloseWindow();    
}
