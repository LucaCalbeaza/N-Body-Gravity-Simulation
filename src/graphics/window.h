/** 
 * File: window.h
 * Description: Declarations for the Window class. 
*/

#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window {
public: 
    // Program Window
    GLFWwindow* window;

    /**
     * Window Constructor: Creates a window with given width, height
     * and name. Sets the context to this window and the viewport to 
     * match the window dimensions.  
     */
    Window(unsigned int width, unsigned int height, const char* title);

    /**
     * Swap the front and back buffer of the window and then poll for events. 
     * Additonally update the window title to display FPS. 
     */
    void update(const char* title);
   
    /**
     * Process all input: query GLFW whether relevant keys are pressed/released 
     * this frame and react accordingly.
     */
    void processInput();

    /**
     * Ensures that the viewport matches any new window dimensions.
     */
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

    /**
     * Terminates the window.
    */
    void terminate();

};

#endif