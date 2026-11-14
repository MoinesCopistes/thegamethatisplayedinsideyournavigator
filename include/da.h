#pragma once

#define da_append(xs, x) \
    do { \
        if ((xs).count >= (xs).capacity) { \
            size_t _new_cap = (xs).capacity ? (xs).capacity * 2 : 64; \
            void *_tmp = realloc((xs).items, _new_cap * sizeof *(xs).items); \
            if (!_tmp) { fprintf(stderr, "da_append: OOM\n"); abort(); } \
            (xs).items    = _tmp; \
            (xs).capacity = _new_cap; \
        } \
        (xs).items[(xs).count++] = (x); \
    } while (0)

#define da_free(xs) \
    do { free((xs).items); (xs).items = NULL; \
         (xs).count = 0; (xs).capacity = 0; } while (0)

