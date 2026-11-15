#include "level_manager.h"
#include "raylib.h"

<<<<<<< Updated upstream
Platforms get_level(int level_id) {
    Platforms p = {0};
    switch (level_id) {
        case 1:
            da_append(p,
                      ((Platform){
                          (Rectangle){.x = 200, .y = 400, .width = 300, .height = 300},
                          .kill=0, .move = 0, .bumper = 0}));
            da_append(
                p,
                ((Platform){(Rectangle){.x = 600, .y = 200, .width = 200, .height = 30},
                            .kill=0, .move = 1, .min = (Vector2){.x = 550, .y = 200},
                            .max = (Vector2){.x = 800, 200},
                            .speed = (Vector2){.x = 50, .y = 0}, .bumper = 0}));
            da_append(
                p,
                ((Platform){(Rectangle){.x = 500, .y = 800, .width = 200, .height = 30},
                            .kill=0, .move = 0, .bumper = 0}));
            da_append(
                p,
                ((Platform){(Rectangle){.x = 750, .y = 830, .width = 200, .height = 30},
                            .kill=0, .move = 0, .bumper = 0}));
            da_append(
                p,
                ((Platform){(Rectangle){.x = 750, .y = 820, .width = 200, .height = 10},
                            .kill=0, .move = 0, .bumper = 4.5}));
            da_append(p,
                      ((Platform){
                          (Rectangle){.x = 950, .y = 340, .width = 200, .height = 630},
                          .kill=0, .move = 0, .bumper = 0}));
            da_append(p, ((Platform){
                             (Rectangle){.x = 950, .y = 330, .width = 50, .height = 10},
                             .kill=0, .move = 0, .bumper = 3}));
            da_append(p, ((Platform) {
                             (Rectangle){.x = 550, .y = 160, .width = 40, .height = 40},
                             .kill = 0, .move = 0, .bumper = 0, .end = 1
                             }));
            break;
        case 2:
=======



#define RECT(px, py, pw, ph) \
    (Rectangle){ .x = (px), .y = (py), .width = (pw), .height = (ph) }

#define PLATFORM(px, py, pw, ph) \
    ((Platform){ .rect = RECT(px, py, pw, ph) })

#define END_PLATFORM(px, py, pw, ph) \
    ((Platform){ .rect = RECT(px, py, pw, ph), .end = 1 })

#define BUMPER_PLATFORM(px, py, pw, ph, strength) \
    ((Platform){ .rect = RECT(px, py, pw, ph), .bumper = (strength) })

#define KILL_PLATFORM(px, py, pw, ph) \
    ((Platform){ .rect = RECT(px, py, pw, ph), .kill = 1 })

#define MOVING_PLATFORM(px, py, pw, ph, min_x, min_y, max_x, max_y, speed_x, speed_y) \
    ((Platform){ \
        .rect  = RECT(px, py, pw, ph), \
        .speed = (Vector2){ .x = (speed_x), .y = (speed_y) }, \
        .max   = (Vector2){ .x = (max_x),   .y = (max_y)   }, \
        .min   = (Vector2){ .x = (min_x),   .y = (min_y)   }, \
        .move  = 1 \
    })
    
Platforms get_level(int number) {
    Platforms p = {0};
    switch (number) {
        case 0:
            da_append(p, PLATFORM(200, 400, 300, 300));

            da_append(p, MOVING_PLATFORM(
                600, 200, 200, 30, 
                550, 200,  // min
                800, 200,  // max
                50, 0      // speed
            ));

            da_append(p, PLATFORM(500, 800, 200, 30));

            da_append(p, PLATFORM(750, 830, 200, 30));

            // A bumper platform 10 pixels thick
            da_append(p, BUMPER_PLATFORM(750, 820, 200, 10, 4.5));

            da_append(p, PLATFORM(950, 340, 200, 630));

            // Another bumper
            da_append(p, BUMPER_PLATFORM(950, 330, 50, 10, 3));

            // The end goal
            da_append(p, END_PLATFORM(550, 160, 40, 40));
            // da_append(p,
            //           ((Platform){
            //               (Rectangle){.x = 200, .y = 400, .width = 300, .height = 300},
            //               .kill=0, .move = 0, .bumper = 0}));
            // da_append(
            //     p,
            //     ((Platform){(Rectangle){.x = 600, .y = 200, .width = 200, .height = 30},
            //                 .kill=0, .move = 1, .min = (Vector2){.x = 550, .y = 200},
            //                 .max = (Vector2){.x = 800, 200},
            //                 .speed = (Vector2){.x = 50, .y = 0}, .bumper = 0}));
            // da_append(
            //     p,
            //     ((Platform){(Rectangle){.x = 500, .y = 800, .width = 200, .height = 30},
            //                 .kill=0, .move = 0, .bumper = 0}));
            // da_append(
            //     p,
            //     ((Platform){(Rectangle){.x = 750, .y = 830, .width = 200, .height = 30},
            //                 .kill=0, .move = 0, .bumper = 0}));
            // da_append(
            //     p,
            //     ((Platform){(Rectangle){.x = 750, .y = 820, .width = 200, .height = 10},
            //                 .kill=0, .move = 0, .bumper = 4.5}));
            // da_append(p,
            //           ((Platform){
            //               (Rectangle){.x = 950, .y = 340, .width = 200, .height = 630},
            //               .kill=0, .move = 0, .bumper = 0}));
            // da_append(p, ((Platform){
            //                  (Rectangle){.x = 950, .y = 330, .width = 50, .height = 10},
            //                  .kill=0, .move = 0, .bumper = 3}));
            // da_append(p, ((Platform) {
            //                  (Rectangle){.x = 550, .y = 160, .width = 40, .height = 40},
            //                  .kill = 0, .move = 0, .bumper = 0, .end = 1
                             // }));
            break;
        case 1:

>>>>>>> Stashed changes
            break;
    
    }
    return p;
}

void unload_level(Platforms *p) {
    if (!p)
        return;
    da_free(*p);
}
