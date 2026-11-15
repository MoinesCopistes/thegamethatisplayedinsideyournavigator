#include "assets/bike_pic.h"
#include "assets/egg.h"
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
#define DEATH_ANIM_DURATION 90

DetectionResult result;
Platforms plat = {0};
Bike *bike;
Texture2D bike_texture;
Texture2D egg;
Texture2D background;
Color bg_color;
int animation_frame_count = 0;
int fading_frame_count = 0;
bool *usage_mask;

bool animation_finished = false;
size_t last_anim_idx = -1;
float accumulator = 0.0f;
int death_frame_count = 0;

static void UpdatePlatforms(float delta_time) {
    for (size_t i = 0; i < plat.count; i++) {
        Platform *platform = &plat.items[i];
        if (!platform->move)
            continue;

        Vector2 old_position = {platform->rect.x, platform->rect.y};

        platform->rect.x += platform->speed.x * delta_time;
        platform->rect.y += platform->speed.y * delta_time;

        if (platform->max.x > platform->min.x) {
            if (platform->rect.x >= platform->max.x) {
                platform->rect.x = platform->max.x;
                platform->speed.x = -fabsf(platform->speed.x);
            } else if (platform->rect.x <= platform->min.x) {
                platform->rect.x = platform->min.x;
                platform->speed.x = fabsf(platform->speed.x);
            }
        }

        if (platform->max.y > platform->min.y) {
            if (platform->rect.y >= platform->max.y) {
                platform->rect.y = platform->max.y;
                platform->speed.y = -fabsf(platform->speed.y);
            } else if (platform->rect.y <= platform->min.y) {
                platform->rect.y = platform->min.y;
                platform->speed.y = fabsf(platform->speed.y);
            }
        }

        Vector2 movement = {platform->rect.x - old_position.x,
                            platform->rect.y - old_position.y};

        for (size_t j = 0; j < result.count; j++) {
            if (!usage_mask[j] || result.items[j].platform != platform)
                continue;

            result.items[j].final_pos.x += movement.x;
            result.items[j].final_pos.y += movement.y;
            result.items[j].rect.x += movement.x;
            result.items[j].rect.y += movement.y;
        }
    }
}

void DrawBike(Bike *bike) {
    Vector2 diff = {.x = (bike->w2.x - bike->w1.x),
                    .y = (bike->w2.y - bike->w1.y)};
    double angle = atan2((double)diff.y, (double)diff.x);
    Vector2 origin = {.x = bike_texture.width / 4.0f,
                      .y = 3.0 * bike_texture.height / 4.0f};

    Rectangle source = {0, 0, bike_texture.width, bike_texture.height};
    Rectangle dest = {bike->w1.x + 10, bike->w1.y, bike_texture.width,
                      bike_texture.height};

    // Draw the bike normally (or fade it out if dead)
    if (!bike->dead) {
        DrawTexturePro(bike_texture, source, dest, origin, angle * RAD2DEG,
                       WHITE);

        Vector2 bike_axis = {(float)cos(angle), (float)sin(angle)};
        Vector2 bike_normal = {-bike_axis.y, bike_axis.x};
        Vector2 egg_anchor = {
            .x = bike->w1.x + 15.0f * bike_axis.x - 31.0f * bike_normal.x,
            .y = bike->w1.y + 15.0f * bike_axis.y - 31.0f * bike_normal.y,
        };
        Rectangle egg_source = {0, 0, egg.width, egg.height};
        Rectangle egg_dest = {egg_anchor.x, egg_anchor.y, egg.width,
                              egg.height};
        Vector2 egg_origin = {egg.width / 2.0f, egg.height};
        DrawTexturePro(egg, egg_source, egg_dest, egg_origin, angle * RAD2DEG,
                       WHITE);
    } else if (death_frame_count < 30) {
        float alpha = 1.0f - ((float)death_frame_count / 30.0f);
        DrawTexturePro(bike_texture, source, dest, origin, angle * RAD2DEG,
                       Fade(WHITE, alpha));
    }

     if (bike->dead) return; 

    float frameTime = GetFrameTime();
    frameTime =
        fminf(frameTime, 0.25f); // Avoid huge catch-up after a lag spike.
    accumulator += frameTime;

    while (accumulator >= BIKE_DT) {
        step(bike, plat);
        accumulator -= BIKE_DT;
    }

    if (IsKeyDown(KEY_RIGHT))
        bike->throttle = 1;
    else if (IsKeyDown(KEY_LEFT))
        bike->throttle = -1;
    else
        bike->throttle = 0;

    if (IsKeyDown(KEY_UP))
        bike->torque = 10;
    else if (IsKeyDown(KEY_DOWN))
        bike->torque = -10;
    else
        bike->torque = 0;

    if (IsKeyPressed(KEY_SPACE))
        jump(bike);
}

void GoToNextLevel(Rectangle rect) {
    if (CheckCollisionCircleRec((Vector2){.x = bike->w1.x, .y = bike->w1.y},
                                bike->radius, rect) ||
        CheckCollisionCircleRec((Vector2){.x = bike->w2.x, .y = bike->w2.y},
                                bike->radius, rect)) {
        EM_ASM({ window.parent.postMessage({type : "mousse-next"}, "*"); });
    }
}

void UpdateDrawFrame() {
    animation_frame_count++;
    if (bike->dead) {
        if (death_frame_count < DEATH_ANIM_DURATION) {
            death_frame_count++;

#if defined(PLATFORM_WEB)
            if (death_frame_count == DEATH_ANIM_DURATION) {
                EM_ASM({
                    window.parent.postMessage({type : "mousse-dead"}, "*");
                });
            }
#endif
        }
    } else {
        death_frame_count = 0;
    }

    if (animation_finished)
        UpdatePlatforms(GetFrameTime());

    BeginDrawing();
    DrawTexture(background, 0, 0, WHITE);

    if (animation_finished && fading_frame_count < 50) {
        fading_frame_count++;
    }

    if (fading_frame_count > 0) {
        float alpha = (float)fading_frame_count / 50.0f;
         DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(bg_color, alpha));
    }

    int stagger_delay = 1;
    int anim_duration = 30;
    int active_index = 0;
    int modulo = 128;
    int countDone = 0;

    for (size_t i = 0; i < result.count; i++) {
        if (!usage_mask[i]) {
            countDone++;
            continue;
        }

        int start_frame = active_index * stagger_delay;
        int local_frame = animation_frame_count - start_frame;
        double percentage = ((double)local_frame / (double)anim_duration);
        percentage = percentage * percentage; // quadratic
        active_index++;
        active_index = active_index % modulo;
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
            countDone++;
            float alpha =
                bike->dead
                    ? (1.0f - ((float)death_frame_count / DEATH_ANIM_DURATION))
                    : 1.0f;
            DrawRectangleV((Vector2){.x = reg.final_pos.x - BG_GAP / 2.0f,
                                     .y = reg.final_pos.y - BG_GAP / 2.0f},
                           (Vector2){.x = reg.texture.width + BG_GAP,
                                     .y = reg.texture.height + BG_GAP},
                           Fade(bg_color, alpha));
        }
    }

    if (countDone == result.count) {
        animation_finished = true;
    }

    active_index = 0;
    for (size_t i = 0; i < result.count; i++) {
        if (!usage_mask[i])
            continue;

        int start_frame = active_index * stagger_delay;
        int local_frame = animation_frame_count - start_frame;
        active_index++;
        active_index = active_index % modulo;
        Region reg = result.items[i];

        if (local_frame <= 0) {
            DrawTextureV(reg.texture, reg.pos, WHITE);
        } else if (local_frame < anim_duration) {
            Vector2 pos =
                update_rec_position(local_frame, anim_duration, i, &result);
            DrawTextureV(reg.texture, pos, WHITE);
        } else if (!bike->dead) {
            if (reg.platform->bumper) {
                DrawTextureV(reg.texture, reg.final_pos, RED);
            } else if (reg.platform->end) {
                Rectangle rec = (Rectangle){.x = reg.final_pos.x,
                                            .y = reg.final_pos.y,
                                            .height = reg.rect.height,
                                            .width = reg.rect.width};
                DrawRectangleRec(rec, GREEN);
                GoToNextLevel(rec);
            } else {
                DrawTextureV(reg.texture, reg.final_pos, WHITE);
            }

        } else {
            if (death_frame_count < DEATH_ANIM_DURATION) {
                Vector2 dead_pos = remove_rec_position(
                    death_frame_count, DEATH_ANIM_DURATION, i, &result);
                DrawTextureV(reg.texture, dead_pos, WHITE);
            } else {
                DrawTextureV(reg.texture, reg.pos, WHITE);
            }
        }
    }

    DrawBike(bike);
    EndDrawing();
}

int main(int argc, char **argv) {
    if (argc < 7) {
        fprintf(stderr, "Usage: %s <height> <width> <r> <g> <b> <level-id>\n",
                argv[0]);
        return 1;
    }

    bike = create_bike(250, 100, 15);

    int screenHeight = atoi(argv[1]);
    int screenWidth = atoi(argv[2]);
    unsigned char r = (unsigned char)atoi(argv[3]);
    unsigned char g = (unsigned char)atoi(argv[4]);
    unsigned char b = (unsigned char)atoi(argv[5]);
    int level_id = atoi(argv[6]);

    bg_color = (Color){r, g, b, 255};
    InitWindow(screenWidth, screenHeight, "Matreggs");
    result = process_screen("/screenshot.png");

    Image bike_image = LoadImageFromMemory(".png", bike_png, bike_png_len);
    ImageResize(&bike_image, 80,
                (double)bike_image.height / (double)bike_image.width * 80);

    int brightness = (0.21 * r) + (0.72 * g) + (0.07 * b);
    if (brightness <= 125)
        ImageColorInvert(&bike_image);
    bike_texture = LoadTextureFromImage(bike_image);

    Image egg_image = LoadImageFromMemory(".png", egg_png, egg_png_len);
    ImageResize(&egg_image, 18,
                (double)egg_image.height / (double)egg_image.width * 18);
    egg = LoadTextureFromImage(egg_image);

    plat = get_level(level_id);
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
    while (!WindowShouldClose()) {
        UpdateDrawFrame();
    }
#endif

    return 0;
}
