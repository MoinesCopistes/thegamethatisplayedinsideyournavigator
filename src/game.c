#include "assets/bike_pic.h"
#include "bike.h"
#include "box_finder.h"
#include "level_assembler.h"
#include "level_manager.h"
#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#define BG_GAP 50.0

DetectionResult result;
Platforms plat = {0};
Bike *bike;
Texture2D bike_texture;
Texture2D background;
Color bg_color;
int animation_frame_count = 0;
int fading_frame_count = 0;
bool *usage_mask;

bool animation_finished = false;
size_t last_anim_idx = -1;
void DrawBike(Bike *bike) {

    Vector2 diff = {.x = (bike->w2.x - bike->w1.x),
                    .y = (bike->w2.y - bike->w1.y)};
    double angle = atan2((double)diff.y, (double)diff.x);
    Vector2 origin = {.x = bike_texture.width / 4.0f,
                      .y = 3.0 * bike_texture.height / 4.0f};

    Rectangle source = {0, 0, bike_texture.width, bike_texture.height};

    Rectangle dest = {bike->w1.x + 10, bike->w1.y, bike_texture.width,
                      bike_texture.height};

    DrawTexturePro(bike_texture, source, dest, origin, angle * RAD2DEG, WHITE);

    step(bike, GetFrameTime(), plat);

    if (IsKeyDown(KEY_RIGHT) && bike->on_ground) {
        bike->throttle = 1000;
    } else if (IsKeyDown(KEY_LEFT) && bike->on_ground) {
        bike->throttle = -1000;
    } else {
        bike->throttle = 0;
    }
}

void UpdateDrawFrame() {
    animation_frame_count++;

    BeginDrawing();
    ClearBackground(bg_color);

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
    for (size_t i = 0; i < result.count; i++) {
        if (!usage_mask[i])
            continue;

        int start_frame = active_index * stagger_delay;
        int local_frame = animation_frame_count - start_frame;
        double percentage = ((double)local_frame / (double)anim_duration);
        percentage = percentage * percentage; // quadtratic
        active_index++;
        Region reg = result.items[i];

        if (local_frame <= 0) {
        } else if (local_frame < anim_duration) {
            Vector2 pos =
                update_rec_position(local_frame, anim_duration, i, &result);

            DrawRectangleV(
                (Vector2){
                    .x = (int)((double)pos.x - (BG_GAP / 2.0) * percentage),
                    .y = (int)((double)pos.y - (BG_GAP / 2.0) * percentage)},
                (Vector2){.x = (int)(((double)reg.texture.width + BG_GAP) *
                                     percentage),
                          .y = (int)(((double)reg.texture.height + BG_GAP) *
                                     percentage)},
                bg_color);
        } else {
            DrawRectangleV((Vector2){.x = reg.final_pos.x - BG_GAP / 2.0,
                                     .y = reg.final_pos.y - BG_GAP / 2.0},
                           (Vector2){.x = reg.texture.width + BG_GAP,
                                     .y = reg.texture.height + BG_GAP},
                           bg_color);
        }
    }

    stagger_delay = 4;
    anim_duration = 60;
    active_index = 0;
    for (size_t i = 0; i < result.count; i++) {
        if (!usage_mask[i])
            continue;

        int start_frame = active_index * stagger_delay;
        int local_frame = animation_frame_count - start_frame;
        active_index++;
        Region reg = result.items[i];
        if (local_frame <= 0) {
            DrawTextureV(reg.texture, reg.pos, WHITE);
        } else if (local_frame < anim_duration) {
            Vector2 pos =
                update_rec_position(local_frame, anim_duration, i, &result);
            DrawTextureV(reg.texture, pos, WHITE);
        } else {
            DrawTextureV(reg.texture, reg.final_pos, WHITE);
            if (i == last_anim_idx) {
                animation_finished = true;
            }
        }
    }

    DrawBike(bike);
    EndDrawing();
}

int main(int argc, char **argv) {
    bike = create_bike(200, 200, 40);

    int screenHeight = atoi(argv[1]);
    int screenWidth = atoi(argv[2]);
    unsigned char r = (unsigned char)atoi(argv[3]);
    unsigned char g = (unsigned char)atoi(argv[4]);
    unsigned char b = (unsigned char)atoi(argv[5]);
    unsigned char a = (unsigned char)atoi(argv[6]);

    bg_color = (Color){r, g, b, a};
    InitWindow(screenWidth, screenHeight,
               "raylib [core] example - basic window");
    result = process_screen("/screenshot.png");
    Image bike_image = LoadImageFromMemory(".png", bike_png, bike_png_len);
    ImageResize(&bike_image, 200,
                (double)bike_image.height / (double)bike_image.width * 200);
    bike_texture = LoadTextureFromImage(bike_image);
    printf("texture: %d", bike_texture.height);

    plat = get_level();
    usage_mask = malloc(sizeof(bool) * result.count);
    for (size_t i = 0; i < result.count; i++) {
        usage_mask[i] = 0;
    }
    get_transformations(&plat, &result, usage_mask);
    for (size_t i = 0; i < result.count; i++) {
        if (usage_mask[i])
            last_anim_idx = i;
    }

    Image background_img = LoadImage("/screenshot.png");
    remove_texture_region(&background_img, &result, usage_mask);
    background = LoadTextureFromImage(background_img);
    UnloadImage(background_img);

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else

    {
        UpdateDrawFrame();
    }
#endif

    return 0;
}
