#include "level_manager.h"


Platforms get_level() {
    Platforms p = {0};
    da_append(p, ((Platform){ (Rectangle){.x = 100, .y = 100, .width = 100, .height = 100}, 0  ,  .bumper = 0}));
    da_append(p, ((Platform){ (Rectangle){.x = 200, .y = 400, .width = 300, .height = 300}, 0  ,  .bumper = 0}));
    da_append(p, ((Platform){ (Rectangle){.x = 600, .y = 200, .width = 200, .height = 30} , 0  ,  .bumper = 0}));
    da_append(p, ((Platform){ (Rectangle){.x = 500, .y = 800, .width = 200, .height = 30} , 0  ,  .bumper = 0}));
    da_append(p, ((Platform){ (Rectangle){.x = 750, .y = 830, .width = 200, .height = 30} , 0  ,  .bumper = 0}));
    da_append(p, ((Platform){ (Rectangle){.x = 750, .y = 820, .width = 200, .height = 10} , 0  ,  .bumper = 4.5}));
    da_append(p, ((Platform){ (Rectangle){.x = 950, .y = 340, .width = 200, .height = 630}, 0  ,  .bumper = 0}));
    da_append(p, ((Platform){ (Rectangle){.x = 950, .y = 330, .width = 50 , .height = 10 }, 0  ,  .bumper = 3}));
    return p;
}


void unload_level(Platforms *p) {
    if (!p) return;
    da_free(*p);
}
