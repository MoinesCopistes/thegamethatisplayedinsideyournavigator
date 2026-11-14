#include "raylib-6.2/src/raylib.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

const int screenWidth = 800;
const int screenHeight = 450;

void UpdateDrawFrame() {

  BeginDrawing();

  ClearBackground(RAYWHITE);

  DrawText("Congrats! You created your first window and it works !", 190, 200,
           20, LIGHTGRAY);

  EndDrawing();
}

int main(void) {
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
