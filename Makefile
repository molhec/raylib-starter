# ===================== Cross-platform Makefile (macOS + Windows 11) =====================

.PHONY: all clean run

# ---- Project ----
PROJECT_NAME ?= game
PLATFORM     ?= PLATFORM_DESKTOP

# ---- Detect OS ----
# Windows with MinGW/MSYS/CMD sets OS=Windows_NT
ifeq ($(OS),Windows_NT)
  TARGET_OS := WINDOWS
else
  UNAME_S := $(shell uname -s)
  ifeq ($(UNAME_S),Darwin)
    TARGET_OS := MAC
  else ifeq ($(UNAME_S),Linux)
    TARGET_OS := LINUX
  else
    TARGET_OS := OTHER
  endif
endif

# ---- Compiler ----
CC := g++
ifeq ($(TARGET_OS),MAC)
  CC := clang++
endif

# ---- Build mode ----
BUILD_MODE ?= DEBUG
CFLAGS := -Wall -std=c++17 -D_DEFAULT_SOURCE -Wno-missing-braces
ifeq ($(BUILD_MODE),DEBUG)
  CFLAGS += -g -O0
else
  CFLAGS += -s -O2
endif

# ---- Sources (all .cpp in root and lib/) ----
SRCS := $(wildcard *.cpp) $(wildcard lib/*.cpp)

# ---- Includes ----
INCLUDE_PATHS := -I. -Ilib

# On macOS, prefer Homebrew includes if present
ifeq ($(TARGET_OS),MAC)
  ifneq ($(wildcard /opt/homebrew/include/.*),)
    INCLUDE_PATHS += -I/opt/homebrew/include
  endif
endif

# On Windows, set where raylib is installed (override from env/CLI if needed)
# Examples:
#   set RAYLIB_ROOT=C:/raylib/w64devkit/x86_64-w64-mingw32      (raylib official kit)
#   set RAYLIB_ROOT=C:/msys64/mingw64                           (MSYS2)
RAYLIB_ROOT ?=
ifeq ($(TARGET_OS),WINDOWS)
  ifneq ($(RAYLIB_ROOT),)
    INCLUDE_PATHS += -I$(RAYLIB_ROOT)/include
  endif
endif

# ---- Linker paths ----
LDFLAGS := -L.

ifeq ($(TARGET_OS),MAC)
  ifneq ($(wildcard /opt/homebrew/lib/.*),)
    LDFLAGS += -L/opt/homebrew/lib
  endif
endif

ifeq ($(TARGET_OS),WINDOWS)
  ifneq ($(RAYLIB_ROOT),)
    LDFLAGS += -L$(RAYLIB_ROOT)/lib
  endif
endif

# ---- Libraries per-OS ----
ifeq ($(TARGET_OS),MAC)
  LDLIBS := -lraylib -framework OpenGL -framework OpenAL -framework Cocoa -framework IOKit
else ifeq ($(TARGET_OS),WINDOWS)
  # MinGW libraries for raylib on Windows
  LDLIBS := -lraylib -lopengl32 -lgdi32 -lwinmm
else ifeq ($(TARGET_OS),LINUX)
  LDLIBS := -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
else
  LDLIBS := -lraylib
endif

# ---- Default target ----
all: $(PROJECT_NAME)

$(PROJECT_NAME): $(SRCS)
	$(CC) -o $@ $(SRCS) $(CFLAGS) $(INCLUDE_PATHS) $(LDFLAGS) $(LDLIBS) -D$(PLATFORM)

run: all
	./$(PROJECT_NAME)

clean:
	@rm -f $(PROJECT_NAME) *.o 2>/dev/null || true
	@echo Cleaning done