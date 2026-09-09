/** 
 * File: imGUI.h
 * Description: Declarations for the imGUI class. 
*/

#ifndef GUI_H
#define GUI_H

#include "window.h"
#include "../include/imGUI/imgui.h"
#include "../include/imGUI/imgui_impl_glfw.h"
#include "../include/imGUI/imgui_impl_opengl3.h"
#include <vector>
#include <cstring>
#include <random>

class GUI {
public:
    // Simulation Parameters Structure
    struct MagiConfig {
        bool enabled = true;
        std::string name = "Default Name:"; 
        std::string paramFileName; 
        int category = 1.0;
        int magiProfileIndex = 0;
        int componentStarCount = 1; 
        float componentMass = 1.0f; 
        float scaleRadius = 1.0f; 
        float scaleHeight = 1.0f; 
        float extraParam = 1.0f;

        /**
         * magiConfig constructor 
         */
        MagiConfig(std::string name, int category);

        /**
         * Reset Config to default parameters apart from 
         * the category and names. Intended to be called 
         * after category is changed.
         */
        void resetConfig();
    };

    struct InputParameters {
        // Start
        bool startSimulation = false;

        // Window & Camera
        bool simulation3D = true;
        bool window3D = true;
        int cameraCondition = 0;
        
        // Physical Parameters
        int n = 10000;
        float billionSolarMass = 60.0f;
        float genSizeKpc = 15.0f;
        float timeScaleMyrPerSec = 20.0f;
        
        // Computation Method 
        int computationMethod = 0;
        float theta = 0.5;
        
        // Visuals & Render
        int renderMethod = 1;
        float minColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};  
        float maxColor[4] = {1.0f, 1.0f, 1.0f, 1.0f}; 
        float bodyRadius = 0.005f; 

        // Initial Condition
        int startingCondtion = 0;
        int secondaryStartingCondition = 0;  
        std::vector<MagiConfig> magiParameters;
        std::string hdf5FileName = "Generation";
    
        /**
         * Input Parameters constructor: Loads the magiParmeters 
         * vectors with the 7 default components.  
         */
        InputParameters();
    };

    // ------ GUI -------------
    InputParameters parameters;
    bool loadingMAGI = false;
    float sectionSeperation = 0.03f;
    
    // Fonts
    ImFont* titleFont;
    ImFont* sectionFont;
    ImFont* regularFont;

    // Flags
    ImGuiWindowFlags colorPickerFlags = ImGuiColorEditFlags_NoSidePreview
        | ImGuiColorEditFlags_NoInputs
        | ImGuiColorEditFlags_NoAlpha;

    /**
     * imGUI Constructor: Creates a new imGUI window context 
     * onto the given window. 
     */
    GUI(Window &window, InputParameters parameters);

    /**
     * Cycle a new Dear ImGui frame. 
     */
    void cycleFrame();

    /**
     * Renders the new frame draw data 
     */
    void renderFrame();

    /**
     * Run the GUI main menu and return the simulation parameters 
     * when the user clicks the start button. 
     */
    InputParameters runMenu(Window &window, unsigned int guiWidth, unsigned int guiHeight);

    /**
     * 
     */
    void cameraCondition();

    /**
     * 
     */
    void renderAndColorCondition();

    /**
     * 
     */
    void coreSimulationParameters();

    /**
     * 
     */
    void computationMethod();

    /**
     * 
     */
    void standardInitialConditions();

    /**
     * 
     */
    void magiConditions();

    /**
     * 
     */
    void magiComponentParametersWindow(MagiConfig& config);

    /**
     * 
     */
    void updateRestrictions();

    /**
     * 
     */
    void startButton();

    /**
     * Run the MAGI generation loading screen
     */
    void runGeneration(Window &window, unsigned int guiWidth, unsigned int guiHeight);



    /**
     * Terminate the imGUI context 
     */
    void terminate();

};


#endif