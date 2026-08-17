#include <iostream>
#include "simulation.h"
#include "graphics/window.h"
#include "graphics/gui.h"

bool runGUI(Window &window, GUI &gui) {
    bool startSimulation = false;
    while (!glfwWindowShouldClose(window.window) && !startSimulation) {
        glfwPollEvents();

        gui.cycleFrame();

        ImGui::SetNextWindowSize(ImVec2(1000, 1000), ImGuiCond_FirstUseEver);
        ImGui::Begin("N-Body Simulation Setup");

        if (ImGui::Button("Start Simulation", ImVec2(160, 32))) {
            startSimulation = true;
        }

        ImGui::End();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        gui.renderFrame();
        glfwSwapBuffers(window.window);
    }

    return startSimulation;
}

// Main Class
int main() {
    Window window(1000, 1000, "N-Body Orbital Simulation", true);
    GUI gui(window);

    if (!runGUI(window, gui)) {
        window.terminate();
        return 0;
    }

    Simulation simulation(window, 60.0f, 100000, 1.0f, 0.1f, 1.0f, true);
    return 0;
}