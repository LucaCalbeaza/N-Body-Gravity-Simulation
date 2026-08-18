#include <iostream>
#include "simulation.h"
#include "graphics/window.h"
#include "graphics/gui.h"

// Window Parameters
const int guiWidth = 1000;
const int guiHeight = 1000;
const int simulationWidth = 1000;
const int simulationHeight = 1000;
static int cameraCondition = 0;
bool window3D = true;


// Simulation Parameters
bool simulation3D = false;
static int n = 10000;
static float mass = 1.0; 
static float G = 0.1;
static float theta = 0.5;
static int computationMethod = 0;
static int startingCondtion = 0;


bool runGUI(Window &window, GUI &gui) {
    bool startSimulation = false;
    while (!glfwWindowShouldClose(window.window) && !startSimulation) {
        glfwPollEvents();
        gui.cycleFrame();

        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2((float)guiWidth, (float)guiHeight), ImGuiCond_Always);
        ImGuiIO& io = ImGui::GetIO();

        ImFont* titleFont = io.Fonts->AddFontFromFileTTF("fonts/FjallaOne-Regular.ttf", 48.0f);
        ImFont* regularFont = io.Fonts->AddFontFromFileTTF("fonts/FjallaOne-Regular.ttf", 28.0f);

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoCollapse
            | ImGuiWindowFlags_NoTitleBar
            | ImGuiWindowFlags_NoBringToFrontOnFocus;

        ImGui::Begin("N-Body Simulation Setup", nullptr, flags);

        // Title Text
        ImGui::PushFont(titleFont);
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("N-Body Simulation Setup").x) / 2);
        ImGui::Text("N-Body Simulation Setup");
        ImGui::PopFont();

        ImGui::PushFont(regularFont);



        // Camera Condtion
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetWindowSize().y * 0.05f);
        ImGui::TextWrapped("Select Camera Configuration");
        ImGui::Separator();

        if (ImGui::RadioButton("3D Camera", cameraCondition == 0)) { 
            cameraCondition = 0;
            window3D = true;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("2D Camera", cameraCondition == 1)) { 
            cameraCondition = 1; 
            window3D = false;
        }



        // Simulation Parameters
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetWindowSize().y * 0.05f);
        ImGui::TextWrapped("Configure Simulation Parameters");
        ImGui::Separator();

        // N Slider
        ImGui::Text("Number of Particles:");
        ImGui::SameLine(ImGui::GetWindowSize().x * 0.35f); 
        ImGui::PushItemWidth(-1.0f);
        ImGui::SliderInt("##Number of Particles", &n, 1, 250000);
        ImGui::SetItemTooltip("This is a simple text tooltip.");
        ImGui::PopItemWidth();

        // Mass Slider
        ImGui::Text("Total System Mass:");
        ImGui::SameLine(ImGui::GetWindowSize().x * 0.35f); 
        ImGui::PushItemWidth(-1.0f);
        ImGui::SliderFloat("##Total System Mass", &mass, 0.1f, 2.0f);
        ImGui::SetItemTooltip("This is a simple text tooltip.");
        ImGui::PopItemWidth();

        // G Slider
        ImGui::Text("Gravitational Constant G:");
        ImGui::SameLine(ImGui::GetWindowSize().x * 0.35f); 
        ImGui::PushItemWidth(-1.0f);
        ImGui::SliderFloat("##Gravitational Constant G", &G, 0.01f, 1.0f);
        ImGui::SetItemTooltip("This is a simple text tooltip.");
        ImGui::PopItemWidth();


        // Computation Method
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetWindowSize().y * 0.05f);
        ImGui::TextWrapped("Select Computation Method");
        ImGui::Separator();
        
        if (ImGui::RadioButton("Barnes-Hut Tree Computation", computationMethod == 0)) { 
            computationMethod = 0;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Brute Force Computation", computationMethod == 1)) { 
            computationMethod = 1; 
        }

        // Theta Slider
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetWindowSize().y * 0.015f);
        ImGui::Text("Barnes-Hut Theta Value:");
        ImGui::SameLine(ImGui::GetWindowSize().x * 0.35f); 
        ImGui::PushItemWidth(-1.0f);
        ImGui::BeginDisabled(computationMethod == 1);
        ImGui::SliderFloat("##Barnes-Hut Theta Value", &theta, 0.0f, 1.0f);
        ImGui::SetItemTooltip("This is a simple text tooltip.");
        ImGui::EndDisabled();
        ImGui::PopItemWidth();




        // Starting Conditions
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetWindowSize().y * 0.05f);
        ImGui::TextWrapped("Select Starting Conditions");
        ImGui::Separator();
        if (ImGui::RadioButton("Random Generation 2D", startingCondtion == 0)) { 
            startingCondtion = 0;
            simulation3D = false;
        }
        ImGui::SameLine();
        ImGui::BeginDisabled(!window3D);
        if (ImGui::RadioButton("Random Generation 3D", startingCondtion == 1)) { 
            startingCondtion = 1; 
            simulation3D = true;
        }
        ImGui::EndDisabled();


        // Start Button
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::GetWindowSize().x * 0.5f) / 2);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetWindowSize().y * 0.1f);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.4f, 1.0f));
        if (ImGui::Button("Start Simulation", ImVec2(ImGui::GetWindowSize().x * 0.5f, 60))) {
            startSimulation = true;
        }
        ImGui::PopStyleColor();
        ImGui::PopFont();

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
    Window window(guiWidth, guiHeight, "N-Body Orbital Simulation", true);
    GUI gui(window);

    while (!glfwWindowShouldClose(window.window)) {
        glfwSetWindowSize(window.window, guiWidth, guiHeight);

        if (!runGUI(window, gui)) {
            break;
        }
        
        window.resetCamera(window3D);
        glfwSetWindowSize(window.window, simulationWidth, simulationHeight);

        Simulation simulation(window, computationMethod, n, mass, G, theta, simulation3D);
        window.returnToMenu = false;
    }

    window.terminate();
    return 0;
}