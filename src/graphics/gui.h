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

    /**
     * imGUI Constructor: Creates a new imGUI window context 
     * onto the given window. 
     */
    GUI(Window &window);

    /**
     * Cycle a new Dear ImGui frame. 
     */
    void cycleFrame();

    /**
     * Renders the new frame draw data 
     */
    void renderFrame();

    /**
     * Terminate the imGUI context 
     */
    void terminate();

};


#endif