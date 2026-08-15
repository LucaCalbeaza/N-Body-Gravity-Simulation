/** 
 * File: imGUI.h
 * Description: Declarations for the imGUI class. 
*/

#ifndef IMGUI_H
#define IMGUI_H

#include "window.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class imGUI {
public:

    /**
     * imGUI Constructor: Creates a new imGUI window context 
     * onto the given window. 
     */
    imGUI(Window &window);

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