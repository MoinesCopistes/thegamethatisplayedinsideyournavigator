#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "box_finder.h"
#include "da.h"
#include "raylib.h"

typedef struct {
  unsigned char r, g, b, a;
} Pixel;
typedef struct {
  int x, y;
} Point;
typedef struct {
  int x, y, width, height;
} Rect;

#define POS(x, y) ((y) * width + (x))
#define COLOR_THRESHOLD 110
#define MAX_AREA 7500
#define MIN_BOUNDING_BOX_WIDTH 2
#define MIN_BOUNDING_BOX_HEIGHT 5

static float distance_between_pixels(Pixel *a, Pixel *b) {
  int dr = (int)a->r - (int)b->r;
  int dg = (int)a->g - (int)b->g;
  int db = (int)a->b - (int)b->b;
  return sqrtf((float)(dr * dr + dg * dg + db * db));
}

static Rect identify_bounding_box(Point *stack, Pixel *img, int start_x,
                                  int start_y, int width, int height,
                                  char *covered) {
  Rect rect = {.x = start_x, .y = start_y, .width = 1, .height = 1};
  size_t top = 0;

  int min_x = start_x, min_y = start_y, max_x = start_x, max_y = start_y;
  int start_idx = POS(start_x, start_y);

  covered[start_idx] = 1;
  stack[top++] = (Point){start_x, start_y};

  while (top > 0) {
    Point current = stack[--top];
    if (current.x < min_x)
      min_x = current.x;
    if (current.x > max_x)
      max_x = current.x;
    if (current.y < min_y)
      min_y = current.y;
    if (current.y > max_y)
      max_y = current.y;

    for (int dx = -1; dx <= 1; dx++) {
      for (int dy = -1; dy <= 1; dy++) {
        if (dx == 0 && dy == 0)
          continue;
        int nx = current.x + dx, ny = current.y + dy;
        if (nx < 0 || ny < 0 || nx >= width || ny >= height)
          continue;
        int neighbor_idx = POS(nx, ny);
        if (covered[neighbor_idx])
          continue;

        int distance =
            distance_between_pixels(&img[start_idx], &img[neighbor_idx]);
        if (distance <= COLOR_THRESHOLD) {
          covered[neighbor_idx] = 1;
          stack[top++] = (Point){nx, ny};
        }
      }
    }
  }
  rect.x = min_x;
  rect.y = min_y;
  rect.width = max_x - min_x + 1;
  rect.height = max_y - min_y + 1;
  return rect;
}

DetectionResult process_screen(const char *path) {
  DetectionResult result = {0};

  Image src = LoadImage(path);
  if (src.data == NULL) {
    fprintf(stderr, "Could not load image: %s\n", path);
    return result;
  }
  ImageFormat(&src, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
  if (src.format != PIXELFORMAT_UNCOMPRESSED_R8G8B8A8) {
    fprintf(stderr, "Could not convert image to RGBA8\n");
    UnloadImage(src);
    return result;
  }

  int width = src.width, height = src.height;
  Pixel *pixels = (Pixel *)src.data;
  size_t pixel_count = (size_t)width * (size_t)height;

  char *covered = calloc(pixel_count, 1);

  Image vis = {0};
  vis.width = width;
  vis.height = height;
  vis.mipmaps = 1;
  vis.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
  vis.data = malloc(pixel_count * sizeof(Pixel));

  if (!covered || !vis.data) {
    fprintf(stderr, "Could not allocate detection arrays\n");
    free(covered);
    free(vis.data);
    UnloadImage(src);
    return result;
  }
  memcpy(vis.data, pixels, pixel_count * sizeof(Pixel));

  int ignored_count = 0;
  Point *stack = malloc(pixel_count * sizeof(Point));
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      if ((y + x) % 5 != 0) {
        continue;
      }
      int idx = POS(x, y);
      if (covered[idx])
        continue;

      Rect rect =
          identify_bounding_box(stack, pixels, x, y, width, height, covered);

      if (rect.width * rect.height > MAX_AREA ||
          rect.width < MIN_BOUNDING_BOX_WIDTH ||
          rect.height < MIN_BOUNDING_BOX_HEIGHT) {
        ignored_count++;
        continue;
      }

      Rectangle r = {(float)rect.x, (float)rect.y, (float)rect.width,
                     (float)rect.height};

      Image crop = ImageFromImage(src, r);
      if (crop.data == NULL) {
        ignored_count++;
        continue;
      }

      Texture2D tex = LoadTextureFromImage(crop);
      UnloadImage(crop);
      if (tex.id == 0) {
        ignored_count++;
        continue;
      }

      da_append(result, ((Region){
                            .rect = r,
                            .texture = tex,
                            .pos = (Vector2){(float)rect.x, (float)rect.y},
                        }));

      Pixel *viz = (Pixel *)vis.data;
      for (int by = rect.y; by < rect.y + rect.height; by++)
        for (int bx = rect.x; bx < rect.x + rect.width; bx++) {
          int bi = POS(bx, by);
          viz[bi] = (Pixel){255, 0, 0, 255};
          covered[bi] = 1;
        }
    }
  }

  printf("Detected %zu regions; ignored %d oversized regions.\n", result.count,
         ignored_count);

  result.debug_image = LoadTextureFromImage(vis);

  free(covered);
  free(stack);
  UnloadImage(vis);
  UnloadImage(src);
  return result;
}

void unload_detection_result(DetectionResult *r) {
  if (!r)
    return;
  for (size_t i = 0; i < r->count; i++)
    UnloadTexture(r->items[i].texture);
  da_free(*r);
  UnloadTexture(r->debug_image);
  r->debug_image = (Texture2D){0};
}
