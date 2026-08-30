/** 
 * File: window.cpp
 * Description: Implementations for the Window class. 
*/

#include "window.h"


Window::Window(unsigned int width, unsigned int height, const char* title, bool window3D) : 
    camera(window3D),
    window3D(window3D),
    width(width),
    height(height),
    lastX(width / 2.0f),
    lastY(height / 2.0f)
    {
    // Initialize GLFW
    glfwInit();
    

    // Let GLFW know version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // Create GLFW window object, set to context, and check for errors
    window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwSetWindowUserPointer(window, this);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    } 

    // Set rendering window to entire screen from (0,0) to (800, 800) & Enable depth buffer
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
}

void Window::resetCamera(bool input3D) {
    glfwSwapBuffers(window);
    window3D = input3D;
    camera.setDefault(input3D);
}

void Window::update(const char* title) {
    glfwSwapBuffers(window);
    glfwPollEvents();
    glfwSetWindowTitle(window, title);
}

void Window::processInput(float dt) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        returnToMenu = true;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(0, dt, window3D);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(1, dt, window3D);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(2, dt, window3D);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(3, dt, window3D);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(4, dt, window3D);
};

void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void Window::mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));

    if (!win->window3D) {
        return;
    }

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    bool mouseDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

    if (!mouseDown) {
        win->lastX = xpos;
        win->lastY = ypos;
        win->firstMouse = true;
        return;
    }

    if (win->firstMouse) {
        win->lastX = xpos;
        win->lastY = ypos;
        win->firstMouse = false;
    }

    float xoffset = xpos - win->lastX;
    float yoffset = win->lastY - ypos;

    win->lastX = xpos;
    win->lastY = ypos;

    win->camera.ProcessMouseMovement(xoffset, yoffset);
}

void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    win->camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void Window::terminate() {
    glfwTerminate();
};