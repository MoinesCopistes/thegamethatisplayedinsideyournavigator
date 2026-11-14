#include "level_manager.h"


Platforms get_level() {
    Platforms p = {0};
    da_append(p, ((Platform){ (Rectangle){.x = 100, .y = 100, .width = 100, .height = 100}, 0 }));
    da_append(p, ((Platform){ (Rectangle){.x = 300, .y = 400, .width = 70, .height = 300}, 0 }));
    return p;
}


void unload_level(Platforms *p) {
    if (!p) return;
    da_free(*p);
}
