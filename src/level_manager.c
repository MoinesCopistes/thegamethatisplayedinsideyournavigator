#include "level_manager.h"
#include "raylib.h"

static Platform solid(float x, float y, float width, float height) {
    return (Platform){
        .rect = (Rectangle){x, y, width, height},
    };
}

static Platform starting(float x, float y, float width, float height) {
    Platform platform = solid(x, y, width, height);
    platform.start = true;
    return platform;
}

static Platform moving(float x, float y, float width, float height, float min_x,
                       float min_y, float max_x, float max_y, float speed_x,
                       float speed_y) {
    Platform platform = solid(x, y, width, height);
    platform.move = true;
    platform.min = (Vector2){min_x, min_y};
    platform.max = (Vector2){max_x, max_y};
    platform.speed = (Vector2){speed_x, speed_y};
    return platform;
}

static Platform bumper(float x, float y, float width, float height,
                       double strength) {
    Platform platform = solid(x, y, width, height);
    platform.bumper = strength;
    return platform;
}

static Platform deadly(float x, float y, float width, float height) {
    Platform platform = solid(x, y, width, height);
    platform.kill = true;
    return platform;
}

static Platform moving_bumper(float x, float y, float width, float height,
                              float min_x, float min_y, float max_x,
                              float max_y, float speed_x, float speed_y,
                              double strength) {
    Platform platform = moving(x, y, width, height, min_x, min_y, max_x,
                               max_y, speed_x, speed_y);
    platform.bumper = strength;
    return platform;
}

static Platform finish(float x, float y, float width, float height) {
    Platform platform = solid(x, y, width, height);
    platform.end = true;
    return platform;
}

Platforms get_level(int level_id) {
    Platforms p = {0};

    switch (level_id) {

    case 1:

        da_append(p, starting(150, 300, 320, 50));

        da_append(p, solid(550, 300, 100, 45));
        da_append(p, bumper(750, 300, 100, 45, 3));
        da_append(p, solid(1000, 300, 200, 45));

        da_append(p, finish(1250, 300, 170, 60));
        break;
    case 2:
        // Ascenseur: use the vertical platform to reach the upper route.
        da_append(p, starting(150, 300, 300, 50));
        da_append(p, bumper(400, 290, 120, 10, 4));
        da_append(p, moving(540, 360, 180, 35, 540, 180, 540, 520, 0, -65));
        da_append(p, solid(760, 190, 330, 45));
        da_append(p, moving(650, 500, 180, 35, 500, 500, 850, 500, 70, 0));
        da_append(p, solid(260, 650, 300, 45));
        da_append(p, bumper(500, 640, 100, 10, 5));
        da_append(p, finish(900, 130, 170, 60));
        break;

    case 3:
        // Stairway: learn to jump across progressively higher ledges.
        da_append(p, starting(150, 320, 300, 50));
        da_append(p, solid(480, 400, 180, 40));
        da_append(p, bumper(620, 390, 120, 10, 3.5));
        da_append(p, solid(740, 310, 190, 40));
        da_append(p, moving(930, 250, 170, 35, 930, 180, 930, 420, 0, 55));
        da_append(p, solid(700, 600, 400, 45));
        da_append(p, finish(940, 540, 160, 60));
        break;

    case 4:
        da_append(p, starting(200, 400, 300, 300));

        da_append(p, moving(540, 420, 120, 30, 540, 420, 760, 420, 60, 0));
        da_append(p, solid(900, 440, 120, 30));

        da_append(p, moving_bumper(1080, 425, 100, 30, 1080, 250, 1080,
                                   600, 0, 70, 3));
        da_append(p, solid(900, 220, 120, 30));

        da_append(p, solid(640, 220, 220, 30));
        da_append(p, finish(560, 180, 40, 40));
        break;

    case 5:
    default:
        da_append(p, starting(200, 400, 300, 300));

        da_append(p, solid(560, 760, 120, 30));
        da_append(p, bumper(560, 750, 120, 10, 3.5));
        da_append(p, solid(740, 500, 100, 30));

        da_append(p, moving(880, 480, 100, 30, 880, 480, 1000, 480, 70, 0));
        da_append(p, solid(1120, 620, 80, 30));
        da_append(p, bumper(1120, 610, 80, 10, 4));
        da_append(p, solid(900, 270, 140, 30));

        da_append(p, moving(760, 250, 100, 30, 760, 160, 760, 340, 0, 60));
        da_append(p, solid(540, 240, 160, 30));
        da_append(p, deadly(620, 220, 30, 20));
        da_append(p, finish(440, 200, 40, 40));
        break;
    }

    return p;
}

void unload_level(Platforms *p) {
    if (!p)
        return;
    da_free(*p);
}
