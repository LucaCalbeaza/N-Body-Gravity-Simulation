/** 
 * File: window.cpp
 * Description: Implementations for the Window class. 
*/

#include "window.h"


Window::Window(unsigned int width, unsigned int height, const char* title) {
    // Initialize GLFW
    glfwInit();

    // Let GLFW know version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create GLFW window object, set to context, and check for errors
    window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    } 

    // Set rendering window to entire screen from (0,0) to (800, 800)
    glViewport(0, 0, width, height);
}

void Window::update(const char* title) {
    glfwSwapBuffers(window);
    glfwPollEvents();
    glfwSetWindowTitle(window, title);
}

void Window::processInput() {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
};

void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void Window::terminate() {
    glfwTerminate();
};