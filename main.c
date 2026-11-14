#include <stdio.h>
#include <stdlib.h>

#include <math.h>
#include <stdint.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

typedef struct {
  unsigned char r, g, b, a;
} Pixel;

typedef struct {
  int x;
  int y;
} Point;

typedef struct {
  int x;
  int y;
  int width;
  int height;
} Rect;

#define POS(x, y) ((y) * width + (x))

#define COLOR_THRESHOLD 110

/* Bounding boxes larger than either limit are detected, but not reported. */
/*#define MAX_BOUNDING_BOX_WIDTH 120
#define MAX_BOUNDING_BOX_HEIGHT 120 */
#define MAX_AREA 7500
#define MIN_BOUNDING_BOX_WIDTH 2
#define MIN_BOUNDING_BOX_HEIGHT 5

float distance_between_pixels(Pixel *a, Pixel *b) {
  int dr = (int)a->r - (int)b->r;
  int dg = (int)a->g - (int)b->g;
  int db = (int)a->b - (int)b->b;

  return sqrtf((float)(dr * dr + dg * dg + db * db));
}

Rect identify_bounding_box(Pixel *img, int start_x, int start_y, int width,
                           int height, char *visited) {
  Rect rect = {.x = start_x, .y = start_y, .width = 1, .height = 1};

  size_t capacity = 1024;
  size_t top = 0;

  Point *stack = malloc(capacity * sizeof(Point));

  if (!stack) {
    fprintf(stderr, "Could not allocate stack\n");
    return rect;
  }

  int min_x = start_x;
  int min_y = start_y;
  int max_x = start_x;
  int max_y = start_y;

  int start_idx = POS(start_x, start_y);

  visited[start_idx] = 1;
  stack[top++] = (Point){start_x, start_y};

  while (top > 0) {
    Point current = stack[--top];

    // Update bounding box
    if (current.x < min_x)
      min_x = current.x;
    if (current.x > max_x)
      max_x = current.x;
    if (current.y < min_y)
      min_y = current.y;
    if (current.y > max_y)
      max_y = current.y;

    // Explore 8-connected neighbors
    for (int dx = -1; dx <= 1; dx++) {
      for (int dy = -1; dy <= 1; dy++) {

        if (dx == 0 && dy == 0)
          continue;

        int nx = current.x + dx;
        int ny = current.y + dy;

        // Boundary checks
        if (nx < 0 || ny < 0 || nx >= width || ny >= height)
          continue;

        int neighbor_idx = POS(nx, ny);

        if (visited[neighbor_idx])
          continue;

        int distance =
            distance_between_pixels(&img[start_idx], &img[neighbor_idx]);

        if (distance <= COLOR_THRESHOLD) {

          // Mark before pushing to avoid duplicates
          visited[neighbor_idx] = 1;

          // Grow stack if necessary
          if (top >= capacity) {
            capacity *= 2;

            Point *tmp = realloc(stack, capacity * sizeof(Point));

            if (!tmp) {
              fprintf(stderr, "Stack allocation failed\n");
              free(stack);
              return rect;
            }

            stack = tmp;
          }

          stack[top++] = (Point){nx, ny};
        }
      }
    }
  }

  free(stack);

  rect.x = min_x;
  rect.y = min_y;
  rect.width = max_x - min_x + 1;
  rect.height = max_y - min_y + 1;

  return rect;
}

int main(void) {
  int width, height, channels;

  // Force RGBA (4 channels)
  unsigned char *data = stbi_load("wiki.png", &width, &height, &channels, 4);

  if (!data) {
    fprintf(stderr, "Could not load image: %s\n", stbi_failure_reason());
    return 1;
  }

  Pixel *pixels = (Pixel *)data;

  printf("Image: %d x %d\n", width, height);

  /*
   * visited belongs to one flood fill and is reset for every new seed
   * colour. covered persists between fills and prevents starting another
   * search from a pixel that is already part of a detected region/box.
   */
  char *visited = calloc((size_t)width * height, sizeof(char));
  char *covered = calloc((size_t)width * height, sizeof(char));
  Pixel *visualization = malloc((size_t)width * height * sizeof(Pixel));

  if (!visited || !covered || !visualization) {
    fprintf(stderr, "Could not allocate detection arrays\n");
    free(visited);
    free(covered);
    free(visualization);
    stbi_image_free(data);
    return 1;
  }

  size_t pixel_count = (size_t)width * (size_t)height;
  memcpy(visualization, pixels, pixel_count * sizeof(Pixel));
  int region_count = 0;
  int ignored_count = 0;

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int idx = POS(x, y);

      if (covered[idx])
        continue;

      /* Each seed gets an independent colour-cluster search. */
      memset(visited, 0, pixel_count * sizeof(*visited));

      Rect rect = identify_bounding_box(pixels, x, y, width, height, visited);

      if (rect.width * rect.height > MAX_AREA ||
          rect.width < MIN_BOUNDING_BOX_WIDTH ||
          rect.height < MIN_BOUNDING_BOX_HEIGHT) {
        /*
         * Do not let a huge background box hide smaller foreground
         * regions. Only its actual connected pixels become covered.
         */
        for (size_t i = 0; i < pixel_count; i++) {
          if (visited[i])
            covered[i] = 1;
        }
        ignored_count++;
        continue;
      }

      region_count++;
      /*printf("Region %d: (%d, %d, %d, %d)\n",
             region_count,
             rect.x, rect.y, rect.width, rect.height); */

      /* Paint and cover the entire accepted bounding box. */
      for (int box_y = rect.y; box_y < rect.y + rect.height; box_y++) {
        for (int box_x = rect.x; box_x < rect.x + rect.width; box_x++) {
          int box_idx = POS(box_x, box_y);
          visualization[box_idx] = (Pixel){255, 0, 0, 255};
          covered[box_idx] = 1;
        }
      }
    }
  }

  printf("Detected %d regions; ignored %d oversized regions.\n", region_count,
         ignored_count);

  if (!stbi_write_png("regions_red.png", width, height, 4, visualization,
                      width * (int)sizeof(Pixel))) {
    fprintf(stderr, "Could not write regions_red.png\n");
    free(visited);
    free(covered);
    free(visualization);
    stbi_image_free(data);
    return 1;
  }

  printf("Wrote region visualization to regions_red.png\n");

  free(visited);
  free(covered);
  free(visualization);
  stbi_image_free(data);

  return 0;
}
