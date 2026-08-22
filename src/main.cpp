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
    GUI gui(window);

    while (!glfwWindowShouldClose(window.window)) {
        glfwSetWindowSize(window.window, guiWidth, guiHeight);
        GUI::inputParameters parameters = gui.run(window, guiWidth, guiHeight);

        if (!parameters.startSimulation) {
            break;
        }
        
        window.resetCamera(parameters.window3D);
        glfwSetWindowSize(window.window, simulationWidth, simulationHeight);

        Simulation simulation(window, parameters.computationMethod, parameters.n, parameters.mass, parameters.G, parameters.theta, parameters.simulation3D, parameters.minColor, parameters.maxColor);
        window.returnToMenu = false;
    }

    window.terminate();
    return 0;
}