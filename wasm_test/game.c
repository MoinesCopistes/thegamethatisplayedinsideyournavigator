#include "raylib-6.2/src/raylib.h"
#include <stdio.h>
#include <stdlib.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif


void UpdateDrawFrame() {

  BeginDrawing();

  ClearBackground(BLANK);

  DrawText("Congrats! You created your first window bite bite !", 190, 200, 20,
           RED);

  EndDrawing();
}

int main(int argc, char **argv) {
    printf("hello, world %d %s %s %s\n", argc, argv[0], argv[1], argv[2]); 
    int screenHeight = atoi(argv[1]); 
    int screenWidth = atoi(argv[2]); 
  InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else

  {
    UpdateDrawFrame();
  }
#endif

  return 0;
}
