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

class GUI {
public:
    // Simulation Parameters Structure
    struct inputParameters {
        bool startSimulation = false;
        bool simulation3D = false;
        bool window3D = true;
        int n = 10000;
        float mass = 1.0; 
        float G = 0.1;
        float theta = 0.5;
        int computationMethod = 0;
        int renderMethod = 1;
        int startingCondtion = 0;
        int cameraCondition = 0;
        float minColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};  
        float maxColor[4] = {1.0f, 1.0f, 1.0f, 1.0f}; 
        float bodySize = 0.005f; 
    };

    // ------ GUI -------------
    inputParameters parameters;

    /**
     * imGUI Constructor: Creates a new imGUI window context 
     * onto the given window. 
     */
    GUI(Window &window, inputParameters parameters);

    /**
     * Cycle a new Dear ImGui frame. 
     */
    void cycleFrame();

    /**
     * Renders the new frame draw data 
     */
    void renderFrame();

    /**
     * Run the GUI and return the simulation parameters 
     * when the user clicks the start button. 
     */
    inputParameters run(Window &window, unsigned int guiWidth, unsigned int guiHeight);

    /**
     * Terminate the imGUI context 
     */
    void terminate();

};


#endif