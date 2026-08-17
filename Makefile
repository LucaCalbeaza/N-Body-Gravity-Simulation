# ============================================================
# Incremental build for the N-Body OpenGL project (MinGW/g++)
#
# Key idea: every .cpp is compiled to its own cached .o file
# under build/obj/. Make only recompiles a source file if:
#   - the .o file doesn't exist yet, or
#   - the .cpp is newer than the .o, or
#   - a header it depends on changed (tracked via -MMD -MP)
#
# So a full ImGui rebuild happens exactly once. After that,
# `make` (or your VS Code build task) only recompiles the
# files you actually touched.
# ============================================================

# Force recipes to run through MSYS2's bash instead of cmd.exe,
# so Unix shell syntax (mkdir -p, rm -rf, etc.) works correctly.
# Adjust this path if your MSYS2 install isn't at C:\msys64.
SHELL       := C:/msys64/usr/bin/bash.exe
.SHELLFLAGS := -c

CXX      := g++
CC       := gcc
CXXFLAGS := -g -fopenmp -O3 -std=c++17 -Iinclude -MMD -MP
CFLAGS   := -g -fopenmp -O3 -Iinclude -MMD -MP
LDFLAGS  := -fopenmp -Llib
LDLIBS   := -lglfw3 -lopengl32 -lgdi32 -luser32 -lkernel32

TARGET   := main.exe

# --- Source discovery ---
CPP_SOURCES := $(wildcard src/*.cpp) \
               $(wildcard src/graphics/*.cpp) \
               $(wildcard src/physics/*.cpp) \
               $(wildcard include/imGUI/*.cpp)

C_SOURCES   := src/glad.c

# --- Object files mirror the source tree under build/obj ---
OBJ_DIR     := build/obj
CPP_OBJECTS := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(CPP_SOURCES))
C_OBJECTS   := $(patsubst %.c,$(OBJ_DIR)/%.o,$(C_SOURCES))
OBJECTS     := $(CPP_OBJECTS) $(C_OBJECTS)

# --- Auto-generated dependency files (header tracking) ---
DEPS        := $(OBJECTS:.o=.d)

.PHONY: all clean rebuild

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@ $(LDLIBS)
	@echo "Build complete: $(TARGET)"

# Pattern rule: build/obj/path/to/file.o <- path/to/file.cpp
#
# NOTE: mkdir/rm are invoked as `$(SHELL) -c '...'` rather than bare
# commands. Native Windows GNU Make tries to detect "simple" recipe
# lines (no shell metacharacters) and launches them directly via
# CreateProcess, bypassing SHELL entirely — which fails since rm/mkdir
# aren't Windows .exe files. Explicitly naming $(SHELL) as the command
# forces make to spawn bash.exe (a real executable) every time.
$(OBJ_DIR)/%.o: %.cpp
	@$(SHELL) -c 'mkdir -p "$(dir $@)"'
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: %.c
	@$(SHELL) -c 'mkdir -p "$(dir $@)"'
	$(CC) $(CFLAGS) -c $< -o $@

# Pull in auto-generated header dependencies, if they exist
-include $(DEPS)

clean:
	$(SHELL) -c 'rm -rf $(OBJ_DIR) $(TARGET)'

rebuild: clean all
