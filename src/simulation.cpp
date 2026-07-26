/** 
 * File: simulation.cpp
 * Description: Implementations for the Simulation class. 
*/

#include "simulation.h"
#include <iostream>


Simulation::Simulation(unsigned int screenWidth, unsigned int screenHeight, float fps, unsigned int n, float mass, float G) :   
    window(screenWidth, screenHeight, "N-Body Orbital Simulation"),
    shader("src/shaders/vertexShader.txt", "src/shaders/fragmentShader.txt"),
    mesh(std::vector<float>(), std::vector<unsigned int>()),
    n(n),
    mass(mass),
    G(G),
    screenSize(screenWidth)               {
    
    // Time Variables : FPS Update Rate
    dt = 1.0 / fps; 
    lastFrameTime = glfwGetTime();
    
    // Generate Mesh and Star Data
    generateMesh();
    generateStarData();

    // Run Simulation
    run();
}

Mesh Simulation::generateMesh() {
    mesh.createCircle(0.5f, 100, 1.0f, 1.0f, 1.0f);
    return mesh;
}

void Simulation::generateStarData() {
    std::random_device randomDevice;
    std::mt19937 gen(randomDevice());
    std::uniform_real_distribution<float> genRandom(-0.8f, 0.8f);

    for (int i = 0; i < n; i++) {
        glm::vec3 position = glm::vec3(genRandom(gen),  genRandom(gen),  0);
        glm::vec3 veloctiy = glm::vec3(0.1*genRandom(gen),  0.0f,  0.0f);
        glm::vec3 acceleration = glm::vec3(0.0f,  0.0f,  0.0f);
        Body star(position, veloctiy, acceleration, mass/n);
        stars.push_back(star);
    }
}

void Simulation::run() {
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

        // FPS Count Calculation
        fpsFrameCount++;
        fpsElapsedTime += frameTime;
        if (fpsElapsedTime >= 0.5f) {
            currentFPS = fpsFrameCount / fpsElapsedTime;
            fpsFrameCount = 0;
            fpsElapsedTime = 0.0f;
        }

        while (frameTimeAccumulation >= dt) {
            updatePhysicsBarnesHutTree();
            //updatePhysicsBruteForce();
            frameTimeAccumulation -= dt;
        }

        // Render and active shader program
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        shader.use();

        // Bind VAO and Draw
        for(unsigned int i = 0; i < n; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, stars[i].position);
            model = glm::scale(model, glm::vec3(0.01, 0.01, 0.01));
            mesh.draw(model, shader);
        }

        // Swap buffers and poll for events
        std::string title = "N-Body Orbital Simulation - FPS: " + std::to_string((int)currentFPS);
        window.update(title.c_str());    
    }
    terminate();
}

void Simulation::updatePhysicsBruteForce() {
    for (int a = 0; a < stars.size(); a++) {
        for (int b = a + 1; b < stars.size(); b++) {
            if (a != b) {
                glm::vec3 distance = stars[a].position - stars[b].position;
                stars[a].acceleration -= ((G * stars[b].mass * glm::normalize(distance)) / (float)pow(glm::length(distance) + rSoft, 2));
                stars[b].acceleration += ((G * stars[a].mass * glm::normalize(distance)) / (float)pow(glm::length(distance) + rSoft, 2));
            }
        }
    }

    for (auto& star : stars) {
        star.update(dt);
    }
}

void Simulation::updatePhysicsBarnesHutTree() {
    BarnesHutTree tree(screenSize);

    for (int i = 0; i < stars.size(); i++) {
        tree.insert(0, i, stars);
    }

    for (int i = 0; i < stars.size(); i++) {
        stars[i].acceleration = tree.computeAcceleration(0, i, stars, 0.5f, G, rSoft);
    }

    for (auto& star : stars) {
        star.update(dt);
    }
}

void Simulation::terminate() {
    // De-allocate Resources
    mesh.terminate();
    shader.destroy();

    // Terminate GLFW
    window.terminate();
}
