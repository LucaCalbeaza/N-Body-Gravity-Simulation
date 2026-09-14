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
#include "../physics/starGeneration.h"
#include "../physics/parameters.h"
#include "../physics/body.h"

class GUI {
public:
    // ------ GUI -------------
    Parameters parameters;
    StarGeneration starGen;

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
    GUI(Window &window, Parameters parameters, StarGeneration starGen);

    /**
     * Cycle a new Dear ImGui frame. 
     */
    void cycleFrame();

    /**
     * Renders the new frame draw data. 
     */
    void renderFrame();

    /**
     * Run the GUI main menu and return the simulation parameters 
     * when the user clicks the start button. 
     */
    Parameters runMenu(Window &window, unsigned int guiWidth, unsigned int guiHeight);

    /**
     * Draw the Camera condition (3D vs 2D) section of the GUI.
     */
    void cameraCondition();

    /**
     * Draw the star render condition (Mesh Body vs Point Body) and the 
     * star render color conditions (velocity gradient) section of the GUI.
     */
    void renderAndColorCondition();

    /**
     * Draw the core simulation parameters (N, total mass, galaxy scale, 
     * timescale) section of the GUI.
     */
    void coreSimulationParameters();

    
    /**
     * Draw the computation method (Brute-Force vs Barnes-Hut tree) section
     * of the GUI. 
     */
    void computationMethod();

   /**
     * Draw the initial conditions type (3D vs 2D) and the standard conditions 
     * section of the GUI. The standard conditions are the non-MAGI conditions 
     * such as the random normal distribution generation. 
     */
    void standardInitialConditions();

    /**
     * Draw the MAGI conditions section of the GUI. 
     */
    void magiConditions();

    /**
     * Draw the MAGI component dropdown for the given config of the GUI. 
     */
    void magiComponentParametersWindow(Parameters::MagiConfig& config);

    /**
     * Updated the selection to ensure no invalid conditions exist 
     * (ex. a MAGI disk without a bulge) and to ensure N and mass line up 
     * with the sum of the components. 
     */
    void updateSelections();

    /**
     * Draw the start button for the GUI.
     */
    void startButton();

    /**
     * Run the MAGI generation loading screen.
     */
    void runGeneration(Window &window, unsigned int guiWidth, unsigned int guiHeight);



    /**
     * Terminate the imGUI context. 
     */
    void terminate();

};


#endif