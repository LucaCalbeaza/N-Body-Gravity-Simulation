/** 
 * File: imGUI.cpp
 * Description: Implementations for the imGUI class. 
*/

#include "gui.h"

GUI::GUI(Window &window, inputParameters parameters) {
    // Create Context
    this->parameters = parameters;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    // Match the OpenGL context version/profile set up in the constructor
    ImGui_ImplGlfw_InitForOpenGL(window.window, true);
    ImGui_ImplOpenGL3_Init("#version 430");
}


void GUI::cycleFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}


void GUI::renderFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

GUI::inputParameters GUI::run(Window &window, unsigned int guiWidth, unsigned int guiHeight) {
    parameters.startSimulation = false;
    while (!glfwWindowShouldClose(window.window) && !parameters.startSimulation) {
        glfwPollEvents();
        cycleFrame();

        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2((float)guiWidth, (float)guiHeight), ImGuiCond_Always);
        ImGuiIO& io = ImGui::GetIO();

        ImFont* titleFont = io.Fonts->AddFontFromFileTTF("fonts/FjallaOne-Regular.ttf", 48.0f);
        ImFont* sectionFont = io.Fonts->AddFontFromFileTTF("fonts/FjallaOne-Regular.ttf", 28.0f);
        ImFont* regularFont = io.Fonts->AddFontFromFileTTF("fonts/FjallaOne-Regular.ttf", 20.0f);
        float sectionSeperation = 0.03f;

        ImGuiWindowFlags windowflags = ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoCollapse
            | ImGuiWindowFlags_NoTitleBar
            | ImGuiWindowFlags_NoBringToFrontOnFocus;
        
        ImGuiWindowFlags colorPickerFlags = ImGuiColorEditFlags_NoSidePreview
            | ImGuiColorEditFlags_NoInputs
            | ImGuiColorEditFlags_NoAlpha;

        ImGui::Begin("N-Body Simulation Setup", nullptr, windowflags);

        // Title Text
        ImGui::PushFont(titleFont);
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("N-Body Simulation Setup").x) / 2);
        ImGui::Text("N-Body Simulation Setup");
        ImGui::PopFont();



        // Camera Condtion
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetWindowSize().y * sectionSeperation);
        ImGui::PushFont(sectionFont);
        ImGui::TextWrapped("Select Camera Configuration");
        ImGui::PopFont();

        ImGui::PushFont(regularFont);
        if (ImGui::RadioButton("3D Camera", parameters.cameraCondition == 0)) { 
            parameters.cameraCondition = 0;
            parameters.window3D = true;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("2D Camera", parameters.cameraCondition == 1)) { 
            parameters.cameraCondition = 1; 
            parameters.window3D = false;
            parameters.startingCondtion = 0;
        }
        ImGui::PopFont();
        

        // Particle Graphics and Render Method
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetWindowSize().y * sectionSeperation);
        ImGui::PushFont(sectionFont);
        ImGui::TextWrapped("Select Render Method");
        ImGui::SameLine(ImGui::GetWindowSize().x * 0.73f); 
        ImGui::TextWrapped("Select Velocity Color Gradient");
        ImGui::PopFont();
        ImGui::Separator();

        ImGui::PushFont(regularFont);
        if (ImGui::RadioButton("Mesh Body", parameters.renderMethod == 0)) { 
            parameters.renderMethod = 0;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Point Body", parameters.renderMethod == 1)) { 
            parameters.renderMethod = 1;
        }
        ImGui::SameLine(ImGui::GetWindowSize().x * 0.77f);
        ImGui::ColorEdit4("Min Velocity", parameters.minColor, colorPickerFlags);  
        ImGui::SameLine();
        ImGui::ColorEdit4("Max Velocity", parameters.maxColor, colorPickerFlags);
        
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetWindowSize().y * 0.01f);
        ImGui::Text("Body Size:");
        ImGui::SameLine(ImGui::GetWindowSize().x * 0.35f); 
        ImGui::PushItemWidth(-1.0f);
        ImGui::SliderFloat("##Body Size", &parameters.bodyRadius, 0.001f, 0.025f);
        ImGui::SetItemTooltip("This is a simple text tooltip.");
        ImGui::PopItemWidth();
        ImGui::PopFont();



        // Simulation Parameters
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetWindowSize().y * sectionSeperation);
        ImGui::PushFont(sectionFont);
        ImGui::TextWrapped("Configure Simulation Parameters");
        ImGui::PopFont();
        ImGui::Separator();

        // N Slider
        ImGui::PushFont(regularFont);
        ImGui::Text("Number of Particles:");
        ImGui::SameLine(ImGui::GetWindowSize().x * 0.35f); 
        ImGui::PushItemWidth(-1.0f);
        ImGui::SliderInt("##Number of Particles", &parameters.n, 1, 250000);
        ImGui::SetItemTooltip("This is a simple text tooltip.");
        ImGui::PopItemWidth();

        // Mass Slider
        ImGui::Text("Total System Mass:");
        ImGui::SameLine(ImGui::GetWindowSize().x * 0.35f); 
        ImGui::PushItemWidth(-1.0f);
        ImGui::SliderFloat("##Total System Mass", &parameters.mass, 0.1f, 2.0f);
        ImGui::SetItemTooltip("This is a simple text tooltip.");
        ImGui::PopItemWidth();

        // G Slider
        ImGui::Text("Gravitational Constant G:");
        ImGui::SameLine(ImGui::GetWindowSize().x * 0.35f); 
        ImGui::PushItemWidth(-1.0f);
        ImGui::SliderFloat("##Gravitational Constant G", &parameters.G, 0.01f, 1.0f);
        ImGui::SetItemTooltip("This is a simple text tooltip.");
        ImGui::PopItemWidth();
        ImGui::PopFont();


        // Computation Method
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetWindowSize().y * sectionSeperation);
        ImGui::PushFont(sectionFont);
        ImGui::TextWrapped("Select Computation Method");
        ImGui::PopFont();
        ImGui::Separator();
        
        ImGui::PushFont(regularFont);
        if (ImGui::RadioButton("Barnes-Hut Tree Computation", parameters.computationMethod == 0)) { 
            parameters.computationMethod = 0;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Brute Force Computation", parameters.computationMethod == 1)) { 
            parameters.computationMethod = 1; 
        }

        // Theta Slider
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetWindowSize().y * 0.015f);
        ImGui::Text("Barnes-Hut Theta Value:");
        ImGui::SameLine(ImGui::GetWindowSize().x * 0.35f); 
        ImGui::PushItemWidth(-1.0f);
        ImGui::BeginDisabled(parameters.computationMethod == 1);
        ImGui::SliderFloat("##Barnes-Hut Theta Value", &parameters.theta, 0.0f, 1.0f);
        ImGui::SetItemTooltip("This is a simple text tooltip.");
        ImGui::EndDisabled();
        ImGui::PopItemWidth();
        ImGui::PopFont();




        // Starting Conditions
        ImGui::PushFont(sectionFont);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetWindowSize().y * sectionSeperation);
        ImGui::TextWrapped("Select Starting Conditions");
        ImGui::PopFont();
        ImGui::Separator();

        ImGui::PushFont(regularFont);
        if (ImGui::RadioButton("3D Condition", parameters.simulation3D == 1)) { 
            parameters.simulation3D = true;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("2D Condition", parameters.simulation3D == 0)) {  
            parameters.simulation3D = false;
        }
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetWindowSize().y * 0.01f);
        ImGui::Separator();

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetWindowSize().y * 0.01f);
        if (ImGui::RadioButton("Random Generation", parameters.startingCondtion == 0)) { 
            parameters.startingCondtion = 0;
        }
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetWindowSize().x * 0.02f);
        ImGui::BeginDisabled(!parameters.window3D || !parameters.simulation3D);
        if (ImGui::RadioButton("Elipitcal Galaxy", parameters.startingCondtion == 1)) { 
            parameters.startingCondtion = 1; 
        }
        ImGui::SameLine();
        ImGui::EndDisabled();

        ImGui::BeginDisabled(parameters.startingCondtion != 1);
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetWindowSize().x * 0.50f, 0.0f));
        ImGui::PushItemWidth(25.0f);
        if (ImGui::BeginCombo("##SliderCombo", 0)) {
            ImGui::Text("Elipitical Galaxy Class: ");
            ImGui::SameLine(ImGui::GetWindowSize().x * 0.35f); 
            ImGui::PushItemWidth(-1.0f);
            ImGui::SliderInt("##SliderInside", &parameters.secondaryStartingCondition, 0, 8);
            ImGui::PopItemWidth();
            ImGui::EndCombo();
        }
        ImGui::EndDisabled();

        


        // Start Button
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::GetWindowSize().x * 0.5f) / 2);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetWindowSize().y * sectionSeperation + 0.10f);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.4f, 1.0f));
        if (ImGui::Button("Start Simulation", ImVec2(ImGui::GetWindowSize().x * 0.5f, 60))) {
            parameters.startSimulation = true;
        }
        ImGui::PopStyleColor();
        ImGui::PopFont();

        ImGui::End();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderFrame();
        glfwSwapBuffers(window.window);
    }

    return parameters;
}

void GUI::terminate() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
