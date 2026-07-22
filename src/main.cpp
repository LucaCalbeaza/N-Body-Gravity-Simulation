#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>
#include "shader.h"
#include "window.h"
#include "mesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


// Main Class

// Window Settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

int main() {
    // Create Window
    Window window(SCR_WIDTH, SCR_HEIGHT, "N-Body Orbital Simulation" );
    
    // Build the Shaders and the shader Program ---------------------
    Shader shader("src/shaders/vertexShader.txt", "src/shaders/fragmentShader.txt");
    
    glm::vec3 positions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        glm::vec3( 0.75f,  0.75f, 0.0f), 
        glm::vec3(-0.75f, 0.75f, 0.0f),  
        glm::vec3(-0.75f, -0.75f, 0.0f),  
        glm::vec3( 0.75f, -0.75f, 0.0f)
    };


    std::vector<float> circleVertices;
    std::vector<unsigned int> circleIndices;
    Mesh mesh(circleVertices, circleIndices);
    mesh.createCircle(0.5f, 100, 1.0f, 0.0f, 0.0f);
    
    // ---------------------------------------

    // While loop runs while the window remains open
    while(!glfwWindowShouldClose(window.window)) {
        // Register Input
        window.processInput();

        // Render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw -----
        // Activate Program
        shader.use();

        // Bind VAO and Draw
        for(unsigned int i = 0; i < 5; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, positions[i]);
            float angle = 20.0f * i; 
            model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
            mesh.draw(model, shader);
        }
        // ----------

        // Swap buffers and poll for events
        window.update();    
    }

    // De-allocate Resources
    mesh.terminate();
    shader.destroy();


    // Terminate GLFW
    window.terminate();
    return 0;
}