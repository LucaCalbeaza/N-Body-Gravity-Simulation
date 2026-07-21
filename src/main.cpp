#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>
#include "shader.h"
#include "window.h"

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
    

    // Vertex Data -----------------------------
    // Triangles
    float vertices[] = {
     0.0f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f,       // Mid
    -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f,       // Top Left
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,       // Bot Left
     0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,       // Top Right 
     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,       // Bot Right
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 2,   // Triangle 1
        0, 3, 4    // Triangle 2
    }; 

    glm::vec3 positions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        glm::vec3( 0.75f,  0.75f, 0.0f), 
        glm::vec3(-0.75f, 0.75f, 0.0f),  
        glm::vec3(-0.75f, -0.75f, 0.0f),  
        glm::vec3( 0.75f, -0.75f, 0.0f)
    };

    
    // Buffers and Array
    unsigned int VBO, VAO, EBO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO); 
    glGenBuffers(1, &EBO); 
    
    // Bind VAO
    glBindVertexArray(VAO);

    // Bind VBO buffer to the vertices & EBO buffer to indices
    glBindBuffer(GL_ARRAY_BUFFER, VBO);  
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);  

    // Configure Vertice attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 
    // ---------------------------------------


    

    // While loop runs while the window remains open
    while(!glfwWindowShouldClose(window.getWindow())) {
        // Register Input
        window.processInput();

        // Render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw -----
        // Activate Program
        shader.use();

        // Bind VAO and Draw
        glBindVertexArray(VAO);
        for(unsigned int i = 0; i < 5; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, positions[i]);
            float angle = 20.0f * i; 
            model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
            unsigned int transformLoc = glGetUniformLocation(shader.ID, "transform");
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model));

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        // ----------

        // Swap buffers and poll for events
        window.update();    
    }

    // De-allocate Resources
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &EBO);
    shader.destroy();


    // Terminate GLFW
    window.terminate();
    return 0;
}