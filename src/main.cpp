#include <iostream>
#include "simulation.h"
#include "graphics/window.h"
#include "graphics/gui.h"
#include "magi/magiGeneration.h"

// Window Parameters
const int guiWidth = 1000;
const int guiHeight = 1000;
const int simulationWidth = 1000;
const int simulationHeight = 1000;

enum class AppState { Menu, GeneratingMagi, Simulating };
AppState state = AppState::Menu;
MagiGeneration magiGen{};

// Main Class
int main() {
    Window window(guiWidth, guiHeight, "N-Body Orbital Simulation", true);
    GUI::InputParameters parameters{};

    while (!glfwWindowShouldClose(window.window)) {
        if (state == AppState::Menu) {
            glfwSetWindowSize(window.window, guiWidth, guiHeight);
            GUI gui(window, parameters);
            parameters = gui.runMenu(window, guiWidth, guiHeight);
            gui.terminate();
            if (parameters.startSimulation) {
                if (parameters.startingCondtion == 2) {
                    magiGen.launchCustomGen(parameters);
                    state = AppState::GeneratingMagi;
                } else {
                    state = AppState::Simulating;
                }
            }
        } else if (state == AppState::GeneratingMagi) {
            GUI gui(window, parameters);
            gui.runGeneration(window, guiWidth, guiHeight);
            gui.terminate();
            if (magiGen.pollComplete()) {
                state = AppState::Simulating;
            }
        } else if (state == AppState::Simulating) {
            Simulation simulation(window, parameters, magiGen);
            window.returnToMenu = false;
            state = AppState::Menu;
        }
    }

    window.terminate();
    return 0;
}