
DIR = ./raylib-6.2/src
BUILD = ./build/

main: build/libraylib.a game.c
	emcc -o build/game.html game.c -Os -Wall build/libraylib.a -I.  -L. -Lbuild/libraylib-a -s USE_GLFW=3 -DPLATFORM_WEB
	cp -r build/game.js build/game.wasm extension

build/libraylib.a: $(DIR)/raylib.h
	mkdir -p $(BUILD)
	emcc -c $(DIR)/rcore.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2     -o $(BUILD)rcore.o
	emcc -c $(DIR)/rshapes.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2   -o $(BUILD)rshapes.o
	emcc -c $(DIR)/rtextures.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2 -o $(BUILD)rtextures.o
	emcc -c $(DIR)/rtext.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2     -o $(BUILD)rtext.o
	emcc -c $(DIR)/rmodels.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2   -o $(BUILD)rmodels.o
	emcc -c $(DIR)/raudio.c -Os -Wall -DPLATFORM_WEB                              -o $(BUILD)raudio.o
	emar rcs build/libraylib.a $(BUILD)rcore.o $(BUILD)rshapes.o $(BUILD)rtextures.o $(BUILD)rtext.o $(BUILD)rmodels.o $(BUILD)raudio.o
