#include <iostream>
#include "simulation.h"
#include "graphics/window.h"
#include "graphics/gui.h"

// Window Parameters
const int guiWidth = 1000;
const int guiHeight = 1000;
const int simulationWidth = 1000;
const int simulationHeight = 1000;



// Main Class
int main() {
    Window window(guiWidth, guiHeight, "N-Body Orbital Simulation", true);
    GUI::inputParameters parameters;

    while (!glfwWindowShouldClose(window.window)) {
        glfwSetWindowSize(window.window, guiWidth, guiHeight);
        GUI gui(window, parameters);
        parameters = gui.run(window, guiWidth, guiHeight);
        gui.terminate();

        if (!parameters.startSimulation) {
            break;
        }

        window.resetCamera(parameters.window3D);
        glfwSetWindowSize(window.window, simulationWidth, simulationHeight);

        Simulation simulation(window, parameters);
        
        window.returnToMenu = false;
    }

    window.terminate();
    return 0;
}