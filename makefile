CC      = cc
CFLAGS  = -Wall -Wextra -g -Iinclude -Ivendor/raylib-6.2/src
LDFLAGS = -Lvendor/raylib-6.2/src
LDLIBS  = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

TARGET  = bin/game
SOURCES = $(wildcard src/*.c)
HEADERS = $(wildcard include/*.h)

RAYLIB_DIR = vendor/raylib-6.2/src
RAYLIB_LIB = $(RAYLIB_DIR)/libraylib.a

.PHONY: all clean FORCE

all: $(TARGET)

$(RAYLIB_LIB): FORCE
	@$(MAKE) -C $(RAYLIB_DIR) PLATFORM=PLATFORM_DESKTOP -s

$(TARGET): $(SOURCES) $(HEADERS) $(RAYLIB_LIB)
	@mkdir -p bin
	$(CC) $(CFLAGS) $(SOURCES) $(LDFLAGS) $(LDLIBS) -o $@

FORCE:

clean:
	rm -rf bin
	@$(MAKE) -C $(RAYLIB_DIR) clean -s
