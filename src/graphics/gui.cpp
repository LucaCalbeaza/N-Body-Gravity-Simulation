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

    // Load Fonts
    titleFont = io.Fonts->AddFontFromFileTTF("fonts/FjallaOne-Regular.ttf", 48.0f);
    sectionFont = io.Fonts->AddFontFromFileTTF("fonts/FjallaOne-Regular.ttf", 28.0f);
    regularFont = io.Fonts->AddFontFromFileTTF("fonts/FjallaOne-Regular.ttf", 20.0f);
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


GUI::inputParameters GUI::runMenu(Window &window, unsigned int guiWidth, unsigned int guiHeight) {
    parameters.startSimulation = false;
    while (!glfwWindowShouldClose(window.window) && !parameters.startSimulation) {
        glfwPollEvents();
        cycleFrame();

        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2((float)guiWidth, (float)guiHeight), ImGuiCond_Always);
        ImGuiIO& io = ImGui::GetIO();
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
        ImGui::SetItemTooltip("3D Camera: Move camera with WASD keys and pan by holding Left Click. Allows for all initial condition generations.");
        ImGui::SameLine();
        if (ImGui::RadioButton("2D Camera", parameters.cameraCondition == 1)) { 
            parameters.cameraCondition = 1; 
            parameters.window3D = false;
            parameters.simulation3D = false;
        }
        ImGui::SetItemTooltip("2D Camera: Move camera with WASD keys. Initial conditions restricted to 2D conditions.");
        ImGui::PopFont();
        

        // Particle Graphics and Render Method
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetWindowSize().y * sectionSeperation);
        ImGui::PushFont(sectionFont);
        ImGui::TextWrapped("Select Render Method");
        ImGui::PopFont();
        ImGui::PushFont(regularFont);
        ImGui::SetItemTooltip("Note: Any slider can be typed into with Ctrl + Left Click.");
        ImGui::PopFont();
        ImGui::SameLine(ImGui::GetWindowSize().x * 0.73f); 
        ImGui::PushFont(sectionFont);
        ImGui::TextWrapped("Select Velocity Color Gradient");
        ImGui::PopFont();
        ImGui::PushFont(regularFont);
        ImGui::SetItemTooltip("Star color is determined by the current velocity clamped between 0 and a max velocity threshold. Color is assigned across a gradient.");
        ImGui::PopFont();
        ImGui::Separator();

        ImGui::PushFont(regularFont);
        if (ImGui::RadioButton("Mesh Body", parameters.renderMethod == 0)) { 
            parameters.renderMethod = 0;
        }
        ImGui::SetItemTooltip("Stars are rendered as single color icosphere meshes.");
        ImGui::SameLine();
        if (ImGui::RadioButton("Point Body", parameters.renderMethod == 1)) { 
            parameters.renderMethod = 1;
        }
        ImGui::SetItemTooltip("Stars are rendered as glowing points (Recommended).");
        ImGui::SameLine(ImGui::GetWindowSize().x * 0.77f);
        ImGui::ColorEdit4("Min Velocity", parameters.minColor, colorPickerFlags);  
        ImGui::SetItemTooltip("Color at the start of the velocity gradient.");
        ImGui::SameLine();
        ImGui::ColorEdit4("Max Velocity", parameters.maxColor, colorPickerFlags);
        ImGui::SetItemTooltip("Color at the end of the velocity gradient.");
        
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetWindowSize().y * 0.01f);
        ImGui::Text("Star Size:");
        ImGui::SameLine(ImGui::GetWindowSize().x * 0.35f); 
        ImGui::PushItemWidth(-1.0f);
        ImGui::SliderFloat("##Star Size", &parameters.bodyRadius, 0.001f, 0.025f);
        ImGui::SetItemTooltip("Visual size of the stars (Not realistic to real world scale). Determines diameter of icospheres for Mesh bodies." 
            "\nDetermines diameter of the halo glow for point bodies. Note: Larger sizes reduce simulation performance for point bodies.");
        ImGui::PopItemWidth();
        ImGui::PopFont();



        // Simulation Parameters
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetWindowSize().y * sectionSeperation);
        ImGui::PushFont(sectionFont);
        ImGui::TextWrapped("Configure Simulation Parameters");
        ImGui::PopFont();
        ImGui::PushFont(regularFont);
        ImGui::SetItemTooltip("Note: Any slider can be typed into with Ctrl + Left Click.");
        ImGui::Separator();

        // N Slider
        ImGui::Text("Number of Stars:");
        ImGui::SameLine(ImGui::GetWindowSize().x * 0.35f); 
        ImGui::PushItemWidth(-1.0f);
        ImGui::SliderInt("##Number of Stars", &parameters.n, 1, 250000);
        ImGui::SetItemTooltip("The number of stars has the largest affect on performance out of any setting. Performance limits will primarily depend on the graphics card.");
        ImGui::PopItemWidth();

        // Solar Mass Slider
        ImGui::Text("Total System Solar Mass (in Billions):");
        ImGui::SameLine(ImGui::GetWindowSize().x * 0.35f); 
        ImGui::PushItemWidth(-1.0f);
        ImGui::SliderFloat("##Total System Solar Mass", &parameters.billionSolarMass, 1.0f, 500.0f);
        ImGui::SetItemTooltip("The total mass of the system is measured in solar mass and is evenly split among all the stars in the system.");
        ImGui::PopItemWidth();

        // Galaxy Size Slider
        ImGui::Text("Galaxy Size (kpc):");
        ImGui::SameLine(ImGui::GetWindowSize().x * 0.35f); 
        ImGui::PushItemWidth(-1.0f);
        ImGui::SliderFloat("##Galaxy Size", &parameters.genSizeKpc, 1.0f, 100.0f);
        ImGui::SetItemTooltip("Approximate diameter of the galaxy generation, will vary dependant on the generation option.");
        ImGui::PopItemWidth();

        // Timescale Slider
        ImGui::Text("Timescale (myr/s):");
        ImGui::SameLine(ImGui::GetWindowSize().x * 0.35f); 
        ImGui::PushItemWidth(-1.0f);
        ImGui::SliderFloat("##Timescale", &parameters.timeScaleMyrPerSec, 1.0f, 200.0f);
        ImGui::SetItemTooltip("Simulation speed in millions of years per second in real time");
        ImGui::PopItemWidth();
        ImGui::PopFont();


        // Computation Method
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetWindowSize().y * sectionSeperation);
        ImGui::PushFont(sectionFont);
        ImGui::TextWrapped("Select Computation Method");
        ImGui::PopFont(); 
        ImGui::PushFont(regularFont);
        ImGui::SetItemTooltip("Barnes-Hut Tree (Improved Performance - Reduced Accuracy): Force computation is performed using a Barnes-Hut Quad/Octree."
                            "\nBrute Force (Reduced Performance - Improved Accuracy): Force computation is performance using a brute force vector iteration");
        ImGui::Separator();
        
        // Barnes-Hut Computation
        if (ImGui::RadioButton("Barnes-Hut Tree Computation", parameters.computationMethod == 0)) { 
            parameters.computationMethod = 0;
        }
        ImGui::SetItemTooltip("Recomended for performance gains for simulations with higher star counts. Higher theta values lead to greater performance"
                            "\ngains but also decreased accuracy. Most simulations tend to see performance gains over the Brute Force Computation at theta > 0.25,"
                            "\nhowever this may vary with each simulation condition.");

        // Barnes-Hut Computation Dropdown Box
        ImGui::BeginDisabled(parameters.computationMethod != 0);
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetWindowSize().x * 0.50f, 0.0f));
        ImGui::PushItemWidth(25.0f);
        ImGui::SameLine();
        if (ImGui::BeginCombo("##Barnes-Hut Theta Value SliderCombo", 0)) {
            ImGui::Text("Barnes-Hut Theta Value: ");
            ImGui::SameLine(ImGui::GetWindowSize().x * 0.35f); 
            ImGui::PushItemWidth(-1.0f);
            ImGui::SliderFloat("##Barnes-Hut Theta Value", &parameters.theta, 0.0f, 1.0f);
            ImGui::PopItemWidth();
            ImGui::EndCombo();
        }
        ImGui::EndDisabled();

        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetWindowSize().x * 0.02f);
        if (ImGui::RadioButton("Brute Force Computation", parameters.computationMethod == 1)) { 
            parameters.computationMethod = 1; 
        }
        ImGui::SetItemTooltip("Recommend for maximum accuracy for simulations with lower star counts. Note: In some rare conditions such as flater ellipitcal"
                            "\ngalaxies, the Brute Force Computation may deliver improved performance over the Barnes-Hut Tree Computation");
        ImGui::PopFont();




        // Starting Conditions
        ImGui::PushFont(sectionFont);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetWindowSize().y * sectionSeperation);
        ImGui::TextWrapped("Select Initial Conditions");
        ImGui::PopFont();
        ImGui::PushFont(regularFont);
        ImGui::SetItemTooltip("Select the simulation dimension and then the initial star generation option. Note: 2D simulations condtions universally have"
                            "\nimproved performance over 3D simulation condtions.");
        ImGui::Separator();

        ImGui::BeginDisabled(!parameters.window3D);
        if (ImGui::RadioButton("3D Condition", parameters.simulation3D == 1)) { 
            parameters.simulation3D = true;
        }
        ImGui::SetItemTooltip("3D conditions generate stars across (x, y, z) dimensions.");
        ImGui::EndDisabled();
        ImGui::SameLine();
        if (ImGui::RadioButton("2D Condition", parameters.simulation3D == 0)) {  
            parameters.simulation3D = false;
        }
        ImGui::SetItemTooltip("2D conditions generate stars across (x, y) dimensions. Several generation options are only available for 3D conditions.");
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetWindowSize().y * 0.01f);
        ImGui::Separator();



        
        // Random Generation Option
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetWindowSize().y * 0.01f);
        if (ImGui::RadioButton("Random Uniform Distribution", parameters.startingCondtion == 0)) { 
            parameters.startingCondtion = 0;
        }
        ImGui::SetItemTooltip("Star postiions generated in a random uniform distribution within the selected shape. Initial velocities are set to 0.");

        // Random Generation Dropdown Box
        ImGui::BeginDisabled(parameters.startingCondtion != 0);
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetWindowSize().x * 0.50f, 0.0f));
        ImGui::PushItemWidth(25.0f);
        ImGui::SameLine();
        if (ImGui::BeginCombo("##RandomGenSliderCombo", 0)) {
            if (ImGui::RadioButton("Cubical", parameters.secondaryStartingCondition == 0)) { 
                parameters.secondaryStartingCondition = 0;
            }
            ImGui::SetItemTooltip("2D Condition: Square     3D Condition: Cube");
            if (ImGui::RadioButton("Spherical", parameters.secondaryStartingCondition == 1)) { 
                parameters.secondaryStartingCondition = 1;
            }
            ImGui::SetItemTooltip("2D Condition: Cirlce     3D Condition: Sphere");
            ImGui::EndCombo();
        }
        ImGui::EndDisabled();
        
        
        
        // Ellipitcal Galaxy Option
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetWindowSize().x * 0.02f);
        ImGui::BeginDisabled(!parameters.window3D || !parameters.simulation3D);
        if (ImGui::RadioButton("Elipitcal Galaxy", parameters.startingCondtion == 1)) { 
            parameters.startingCondtion = 1; 
        }
        ImGui::SetItemTooltip("Star postiions generated across a mass density profile following Plummer's method. Initial velocities are determined"
                            "\nby randomizing from a range capped at the escape velocity and then applying rejection sampling against a isotropic"
                            "\nvelocity probability distribution. These galaxies are generated with the intention of being in a stable state, you"
                            "\nshould not observe any galaxy collapses.");
        ImGui::EndDisabled();

        // Ellipitical Galaxy Dropdown Box
        ImGui::BeginDisabled(parameters.startingCondtion != 1);
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetWindowSize().x * 0.50f, 0.0f));
        ImGui::PushItemWidth(25.0f);
        ImGui::SameLine();
        if (ImGui::BeginCombo("##EllipitcalSliderCombo", 0)) {
            ImGui::Text("Elipitical Galaxy Class: ");
            ImGui::SameLine(ImGui::GetWindowSize().x * 0.35f); 
            ImGui::PushItemWidth(-1.0f);
            ImGui::SliderInt("##EllipitcalSlider", &parameters.secondaryStartingCondition, 0, 7);
            ImGui::SetItemTooltip("Assigns the Hubble ellipitcal galaxy class ranging from E0 to E7. Lower elipitcal galaxies classes are more spherical"
                                "\nin shape while higher classes are flatter and more oval in shape.");
            ImGui::PopItemWidth();
            ImGui::EndCombo();
        }
        ImGui::EndDisabled();

        
        // MAGI Galaxy Option
        ImGui::BeginDisabled(!parameters.window3D || !parameters.simulation3D);
        if (ImGui::RadioButton("MAGI: Many-Component Galaxy Initialiser", parameters.startingCondtion == 2)) { 
            parameters.startingCondtion = 2; 
        }
        ImGui::SetItemTooltip("TBA");
        ImGui::EndDisabled();

        // MAGI Galaxy Dropdown Box
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetWindowSize().y * 0.04f);
        ImGui::BeginDisabled(parameters.startingCondtion != 2);
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetWindowSize().x * 0.65f, 0.0f));
        ImGui::PushItemWidth(ImGui::GetWindowSize().x * 0.65);
        ImGui::SameLine();
        if (ImGui::BeginCombo("##MAGICombo", 0)) {
            ImGui::Text("MAGI Configuration: ");
            //ImGui::SameLine(ImGui::GetWindowSize().x * 0.35f); 
            ImGui::SliderInt("##MAGI Configuration", &parameters.magiProfileIndex, 0, 6);
            ImGui::SetItemTooltip("TBA");
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

void GUI::runGeneration(Window &window, unsigned int guiWidth, unsigned int guiHeight) {
    if (!glfwWindowShouldClose(window.window)) {
        glfwPollEvents();
        cycleFrame();

        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2((float)guiWidth, (float)guiHeight), ImGuiCond_Always);
        ImGuiIO& io = ImGui::GetIO();

        ImGuiWindowFlags windowflags = ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoCollapse
            | ImGuiWindowFlags_NoTitleBar
            | ImGuiWindowFlags_NoBringToFrontOnFocus;
        

        ImGui::Begin("N-Body Simulation Setup", nullptr, windowflags);

        // Title Text
        ImGui::PushFont(titleFont);
        ImGui::SetCursorPosY(ImGui::GetWindowSize().y * 0.5f);
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("MAGI Galaxy is Generating... (This may take a moment)").x) / 2);
        ImGui::Text("MAGI Galaxy is Generating... (This may take a moment)");
        ImGui::PopFont();


        ImGui::End();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderFrame();
        glfwSwapBuffers(window.window);
    }
}

void GUI::terminate() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
