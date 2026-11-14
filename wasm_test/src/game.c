#include "box_finder.h"
#include "raylib-6.2/src/raylib.h"
#include <stdio.h>
#include <stdlib.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif


DetectionResult result;

void UpdateDrawFrame() {
  BeginDrawing();

  ClearBackground(BLANK);

    // DrawTexture(res.debug_image, 0, 0, WHITE);
    for (size_t i = 0; i < result.count; i ++) {
        Region reg = result.items[i];
        DrawTextureV(reg.texture, reg.pos, BLUE);
    }


  EndDrawing();
}

int main(int argc, char **argv) {
  printf("hello, world %d %s %s %s\n", argc, argv[0], argv[1], argv[2]);
  int screenHeight = atoi(argv[1]);
  int screenWidth = atoi(argv[2]);
  InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
  result = process_screen("/screenshot.png"); 

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else

  {
    UpdateDrawFrame();
  }
#endif

  return 0;
}
