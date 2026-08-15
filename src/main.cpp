#include <iostream>
#include "simulation.h"
#include "graphics/window.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


// Main Class
int main() {
    Window window(1000, 1000, "N-Body Orbital Simulation", true);
    Simulation simulation(window, 60.0f, 100000, 1.0f, 0.1f, 1.0f, true);
    return 0;
}