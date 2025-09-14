# ===================== Cross-Platform Makefile (WSL/Linux + macOS) =====================

.PHONY: all clean run info

PROJECT_NAME ?= game
PLATFORM     ?= PLATFORM_DESKTOP

# ---- Detect OS ----
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

# ---- Compiler / Flags ----
CXX      := g++
ifeq ($(TARGET_OS),MAC)
  CXX    := clang++
endif

CXXFLAGS := -Wall -std=c++17 -D_DEFAULT_SOURCE -Wno-missing-braces
BUILD    ?= DEBUG
ifeq ($(BUILD),DEBUG)
  CXXFLAGS += -g -O0
else
  CXXFLAGS += -s -O2
endif

# ---- Sources / Includes ----
SRCS          := $(wildcard *.cpp) $(wildcard lib/*.cpp)
INCLUDE_PATHS := -I. -Ilib
LDFLAGS       :=
LDLIBS        :=

# =============================================================================
#                              LINUX / WSL
# =============================================================================
ifeq ($(TARGET_OS),LINUX)
  # Point these to your local checkout/build (override on CLI if needed)
  RAYLIB_HOME      ?= $(HOME)/raylib
  RAYLIB_INCLUDEDIR?= $(RAYLIB_HOME)/src
  RAYLIB_LIBDIR    ?= $(RAYLIB_HOME)/cmake-build/raylib

  INCLUDE_PATHS    += -I$(RAYLIB_INCLUDEDIR)

  # Try explicit libdir first; if not found, search anywhere under RAYLIB_HOME
  RAYLIB_LIB := \
    $(firstword \
      $(wildcard $(RAYLIB_LIBDIR)/libraylib.a) \
      $(wildcard $(RAYLIB_LIBDIR)/libraylib.so) \
      $(shell find $(RAYLIB_HOME) -type f \( -name libraylib.a -o -name libraylib.so \) 2>/dev/null | head -n1) \
    )
  RAYLIB_DIR := $(dir $(RAYLIB_LIB))

  LINUX_SYS_LIBS := -lGL -lm -lpthread -ldl -lrt -lX11

  ifneq ($(findstring .a,$(RAYLIB_LIB)),)
    LDLIBS  += $(RAYLIB_LIB) $(LINUX_SYS_LIBS)
  else ifneq ($(findstring .so,$(RAYLIB_LIB)),)
    LDFLAGS += -L$(RAYLIB_DIR) -Wl,-rpath,$(RAYLIB_DIR)
    LDLIBS  += -lraylib $(LINUX_SYS_LIBS)
  endif
endif

# =============================================================================
#                                 macOS
# =============================================================================
ifeq ($(TARGET_OS),MAC)
  # Prefer pkg-config if available (brew install raylib pkg-config)
  ifeq ($(shell command -v pkg-config >/dev/null 2>&1 && pkg-config --exists raylib && echo yes),yes)
    CXXFLAGS += $(shell pkg-config --cflags raylib)
    LDLIBS   += $(shell pkg-config --libs raylib)
  else
    BREW_PREFIX ?= $(shell brew --prefix 2>/dev/null)
    ifneq ($(BREW_PREFIX),)
      INCLUDE_PATHS += -I$(BREW_PREFIX)/include
      LDFLAGS      += -L$(BREW_PREFIX)/lib
    else
      ifneq ($(wildcard /opt/homebrew/include),)
        INCLUDE_PATHS += -I/opt/homebrew/include
        LDFLAGS      += -L/opt/homebrew/lib
      endif
      ifneq ($(wildcard /usr/local/include),)
        INCLUDE_PATHS += -I/usr/local/include
        LDFLAGS      += -L/usr/local/lib
      endif
    endif

    LDLIBS += -lraylib \
              -framework Cocoa -framework IOKit -framework CoreVideo \
              -framework OpenGL -framework OpenAL
  endif
endif

# =============================================================================
#                                   Targets
# =============================================================================
all: $(PROJECT_NAME)

# Guard: show a clear message only when building on Linux and lib is missing
$(PROJECT_NAME): $(SRCS)
	@if [ "$(TARGET_OS)" = "LINUX" ] && [ ! -f "$(RAYLIB_LIB)" ]; then \
	  echo "✗ Could not find libraylib.{a,so}."; \
	  echo "  Expected under:"; \
	  echo "    RAYLIB_LIBDIR=$(RAYLIB_LIBDIR)  or anywhere in $(RAYLIB_HOME)"; \
	  echo "  Build raylib with:"; \
	  echo "    cd $(RAYLIB_HOME) && rm -rf cmake-build && mkdir cmake-build && cd cmake-build && \\"; \
	  echo "    cmake .. -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release && cmake --build . -j"; \
	  exit 1; \
	fi
	$(CXX) $(SRCS) -o $@ $(CXXFLAGS) $(INCLUDE_PATHS) $(LDFLAGS) $(LDLIBS) -D$(PLATFORM)

run: all
	./$(PROJECT_NAME)

clean:
	@rm -f $(PROJECT_NAME) *.o 2>/dev/null || true
	@echo Cleaning done

info:
	@echo "TARGET_OS       : $(TARGET_OS)"
	@echo "RAYLIB_HOME     : $(RAYLIB_HOME)"
	@echo "RAYLIB_INCLUDEDIR: $(RAYLIB_INCLUDEDIR)"
	@echo "RAYLIB_LIBDIR   : $(RAYLIB_LIBDIR)"
	@echo "Detected lib    : $(RAYLIB_LIB)"
	@echo "INCLUDE_PATHS   : $(INCLUDE_PATHS)"
	@echo "LDFLAGS         : $(LDFLAGS)"
	@echo "LDLIBS          : $(LDLIBS)"
