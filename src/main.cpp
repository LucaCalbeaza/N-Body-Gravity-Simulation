#include <iostream>
#include "simulation.h"
#include "graphics/window.h"
#include "graphics/gui.h"
#include "physics/starGeneration.h"
#include "physics/parameters.h"

// Window Parameters
const int guiWidth = 1000;
const int guiHeight = 1000;
const int simulationWidth = 1000;
const int simulationHeight = 1000;

enum class AppState { Menu, GeneratingMagi, Simulating };
AppState state = AppState::Menu;
StarGeneration starGen{};

// Main Class
int main() {
    Window window(guiWidth, guiHeight, "N-Body Orbital Simulation", true);
    Parameters parameters{};

    while (!glfwWindowShouldClose(window.window)) {
        if (state == AppState::Menu) {
            glfwSetWindowSize(window.window, guiWidth, guiHeight);
            GUI gui(window, parameters, starGen);
            parameters = gui.runMenu(window, guiWidth, guiHeight);
            gui.terminate();
            if (parameters.startSimulation) {
                state = AppState::Simulating;
            } else if (parameters.startGeneration) {
                starGen.launchCustomGen(parameters);
                state = AppState::GeneratingMagi;
            }
        } else if (state == AppState::GeneratingMagi) {
            GUI gui(window, parameters, starGen);
            gui.runGeneration(window, guiWidth, guiHeight);
            gui.terminate();
            if (starGen.pollComplete()) {
                starGen.annotateHdf5Components("magiGenerations/newGenerations/" + parameters.generationHDF5FileName + ".hdf5", parameters);
                state = AppState::Menu;
            }
        } else if (state == AppState::Simulating) {
            Simulation simulation(window, parameters);
            window.returnToMenu = false;
            state = AppState::Menu;
        }
    }

    window.terminate();
    return 0;
}