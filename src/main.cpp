#include <iostream>
#include "simulation.h"
#include "graphics/window.h"


// Main Class
int main() {
    Window window(1000, 1000, "N-Body Orbital Simulation");
    Simulation simulation(window, 60.0f, 50000, 1.0f, 0.1f, 0.9f, true);
    return 0;
}