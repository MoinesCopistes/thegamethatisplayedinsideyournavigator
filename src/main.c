#include "raylib.h"
#include "box_finder.h"
#include "level_manager.h"


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

    
    SetTargetFPS(60);
    TraceLog(LOG_INFO, "Init window");


    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(RAYWHITE);

            // DrawTexture(res.debug_image, 0, 0, WHITE);
            for (size_t i = 0; i < res.count; i ++) {
                Region reg = res.items[i];
                DrawTextureV(reg.texture, reg.pos, WHITE);
            }
            for (size_t i = 0; i < plat.count; i++) {
                Platform p = plat.items[i];
                DrawRectangleRec(p.rec, RED);
            }

        EndDrawing();
    }
    unload_level(&plat);
    unload_detection_result(&res);
    CloseWindow();    
}
