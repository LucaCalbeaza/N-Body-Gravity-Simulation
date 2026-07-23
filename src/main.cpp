#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <random>
#include <cmath>

#define _USE_MATH_DEFINES
#include <math.h>

#include "shader.h"
#include "window.h"
#include "mesh.h"
#include "body.h"

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
    
    // Create Circle Mesh
    std::vector<float> circleVertices;
    std::vector<unsigned int> circleIndices;
    Mesh mesh(circleVertices, circleIndices);
    mesh.createCircle(0.5f, 100, 1.0f, 0.0f, 0.0f);

    // Generate random star property data
    std::random_device randomDevice;
    std::mt19937 gen(randomDevice());
    std::uniform_real_distribution<float> genRandom(-0.8f, 0.8f);

    std::vector<Body> stars;
    unsigned int n = 10;
    float m = 1.0f;
    const float G = 0.1f;
    float minDistance = 0.05f;
    for (int i = 0; i < n; i++) {
        glm::vec3 position = glm::vec3(genRandom(gen),  genRandom(gen),  0);
        glm::vec3 veloctiy = glm::vec3(0.1*genRandom(gen),  0.0f,  0.0f);
        glm::vec3 acceleration = glm::vec3(0.0f,  0.0f,  0.0f);
        Body star(position, veloctiy, acceleration, m/n);
        stars.push_back(star);
    }

    // Time Variables : 60 FPS Update Rate
    const float dt = 1.0 / 60.0; 
    float frameTimeAccumulation = 0.0;
    float lastFrameTime = glfwGetTime();
        

    // While loop runs while the window remains open
    while(!glfwWindowShouldClose(window.window)) {
        // Register Input
        window.processInput();

        // Frame time calculation
        float currentFrameTime = glfwGetTime();
        float frameTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        if (frameTime > 0.25) {
            frameTime = 0.25;
        }
        frameTimeAccumulation += frameTime;

        while (frameTimeAccumulation >= dt) {
            for (int a = 0; a < stars.size(); a++) {
                for (int b = a + 1; b < stars.size(); b++) {
                    if (a != b) {
                        glm::vec3 distance = stars[a].position - stars[b].position;
                        stars[a].acceleration -= ((G * stars[b].mass * glm::normalize(distance)) / (float)pow(glm::length(distance) + minDistance, 2));
                        stars[b].acceleration += ((G * stars[a].mass * glm::normalize(distance)) / (float)pow(glm::length(distance) + minDistance, 2));
                    }
                }
            }

            for (auto& star : stars) {
                star.update(dt);
            }
            frameTimeAccumulation -= dt;
        }

        // Render and active shader program
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        shader.use();

        // Bind VAO and Draw
        for(unsigned int i = 0; i < n; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, stars[i].position);
            float angle = 20.0f * i; 
            model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, glm::vec3(0.05, 0.05, 0.05));
            mesh.draw(model, shader);
        }

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