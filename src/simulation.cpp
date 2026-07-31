/** 
 * File: simulation.cpp
 * Description: Implementations for the Simulation class. 
*/

#include "simulation.h"
#include <iostream>
#include "omp.h"


Simulation::Simulation(unsigned int screenWidth, unsigned int screenHeight, float fps, unsigned int n, float mass, float G) :   
    window(screenWidth, screenHeight, "N-Body Orbital Simulation"),
    shader("src/shaders/vertexShader.txt", "src/shaders/fragmentShader.txt"),
    computeShader("src/shaders/computeShaderBruteForce.txt"),
    mesh(std::vector<float>(), std::vector<unsigned int>(), n),
    n(n),
    mass(mass),
    G(G),
    screenSize(screenWidth)               {
    
    // Time Variables : FPS Update Rate
    dt = 1.0 / fps; 
    lastFrameTime = glfwGetTime();

    // Reserve Star Variables
    innerBodies.reserve(n);
    outerBodies.reserve(n);
    positions.reserve(n);
    
    // Generate Mesh and Star Data
    generateStarData();
    generateMesh();

    // Run Simulation
    run();
}

void Simulation::generateStarData() {
    // Create RNG device set between [-1.0f, 1.0f]
    std::random_device randomDevice;
    std::mt19937 gen(randomDevice());
    std::uniform_real_distribution<float> genRandom(-1.0f, 1.0f);

    // Generate n stars with random initial {x,y} positions between
    // [-1.0f, 1.0f], and random initial{x,y} velocty between [-0.1f, 0.1f]
    for (int i = 0; i < n; i++) {
        glm::vec3 position = glm::vec3(genRandom(gen),  genRandom(gen),  0);
        glm::vec3 veloctiy = glm::vec3(0.1*genRandom(gen),  0.1*genRandom(gen),  0.0f);
        glm::vec3 acceleration = glm::vec3(0.0f,  0.0f,  0.0f);
        Body star(position, veloctiy, acceleration, mass/n);
        stars.push_back(star);
    }
}

Mesh Simulation::generateMesh() {
    // Create circle mesh and load the star data into the mesh SSBO
    mesh.createCircle(0.0025f, 10, 1.0f, 1.0f, 1.0f);
    mesh.loadBodies(stars);
    return mesh;
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

        // Update the star data. Incase of frame drops update the star data 
        // multiple times to account for loss. Limit set to maxStepsPerFrame
        // to prevent the frame drop from spiraling out of control. 
        int steps = 0;
        while (frameTimeAccumulation >= dt && steps < maxStepsPerFrame) {
            //updatePhysicsBarnesHutTree();
            updatePhysicsComputeShader();
            frameTimeAccumulation -= dt;
            steps++;
            if (steps == maxStepsPerFrame) {
                frameTimeAccumulation = 0;
            }
        }

        // Render and active shader program
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        shader.use();

        // Bind position data and Draw
        // positions.clear();
        // for(unsigned int i = 0; i < n; i++) {
        //     positions.push_back(stars[i].position);
        // }
        // mesh.drawInstanced(positions, shader);

        mesh.drawSSBO();

        // Swap buffers and update window title
        std::string title = "N-Body Orbital Simulation - FPS: " + std::to_string((int)currentFPS) + " - Time: " + std::to_string((int)currentFrameTime);
        window.update(title.c_str());    
    }
    terminate();
}

void Simulation::updatePhysicsBruteForce() {
    // For each star directly sum up the gravitational acceleration from
    // all other stars. Additionaly update the gravitional acceleration 
    // on the other star to prevent duplicate computations. 
    for (int a = 0; a < stars.size(); a++) {
        for (int b = a + 1; b < stars.size(); b++) {
            if (a != b) {
                glm::vec3 distance = stars[a].position - stars[b].position;
                stars[a].acceleration -= ((G * stars[b].mass * glm::normalize(distance)) / (float)pow(glm::length(distance) + rSoft, 2));
                stars[b].acceleration += ((G * stars[a].mass * glm::normalize(distance)) / (float)pow(glm::length(distance) + rSoft, 2));
            }
        }
    }

    // Update the data state of each star
    for (auto& star : stars) {
        star.update(dt);
    }
}

void Simulation::updatePhysicsBarnesHutTree() {
    glm::vec3 centerOfMass = computeCenterOfMass();
    outerBodies.clear();
    innerBodies.clear();

    // Split stars into outer and inner lists
    for (int i = 0; i < stars.size(); i++) {
        glm::vec3 distance = stars[i].position - centerOfMass;
        if (glm::length(distance) > boundaryRadius) {
            outerBodies.push_back(i);
        } else {
            innerBodies.push_back(i);
        }
    }

    // Find the inner star with the max x or y value from the center. Construct 
    // the Barnes-hut tree around the center of mass with that max value as the 
    // size with a minor 5% increment.  
    float maxX = 0, maxY = 0;
    for (int index : innerBodies) {
        maxX = std::max<float>(std::abs(stars[index].position.x), maxX);
        maxY = std::max<float>(std::abs(stars[index].position.y), maxY);
    }
    BarnesHutTree tree(std::max<float>(maxX, maxY) * 1.05f, centerOfMass, n);

    // Insert each inner star into the tree
    for (int i : innerBodies) {
        tree.insert(0, i, stars);
    }

    // Following 3 computations are parallelized over multi threads using OpenMP

    // Compute the acceleration of each inner body using the tree approximation
    #pragma omp parallel for schedule(dynamic, 32)
    for (int i : innerBodies) {
        stars[i].acceleration = tree.computeAcceleration(0, i, stars, 0.5f, G, rSoft);
    }

    // Compute the acceleration of each outer body by treating the inner bodies
    // as a single point mass 
    #pragma omp parallel for schedule(dynamic, 32)
    for (int i : outerBodies) {
        glm::vec3 distance = stars[i].position - tree.nodes[0].centerOfMass;
        float distanceSqr = glm::dot(distance, distance);
        float length = std::sqrt(distanceSqr);
        stars[i].acceleration = ((-G * tree.nodes[0].totalMass * distance / length) / ((length + rSoft) * (length + rSoft)));
    }

    // Update the data state of each star
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < stars.size(); i++) {
        stars[i].update(dt);
    }
}

void Simulation::updatePhysicsComputeShader() {
    // Active the compute shader
    computeShader.use();

    // Initialize the computer shader with the uniform values 
    // of the given physcial terms: n, G, rSoft, dt
    glUniform1i(glGetUniformLocation(computeShader.ID, "numBodies"), n);
    glUniform1f(glGetUniformLocation(computeShader.ID, "G"), G);
    glUniform1f(glGetUniformLocation(computeShader.ID, "rSoft"), rSoft);
    glUniform1f(glGetUniformLocation(computeShader.ID, "dt"), dt);

    // Bind the mesh SSBO, compute the number of 1x256 workgroups, 
    // and execute the compute shader computation on the GPU
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, mesh.SSBO);
    unsigned int numGroups = (n + 255) / 256; 
    glDispatchCompute(numGroups, 1, 1);

    // Compute writes must finish before drawing
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
}

glm::vec3 Simulation::computeCenterOfMass() {
    // Calculate the center of mass of all the stars
    glm::vec3 momentOfMass = glm::vec3(0.0f, 0.0f, 0.0f);
    float totalMass = 0; 

    for (int i = 0; i < stars.size(); i++) {
        momentOfMass += stars[i].position * stars[i].mass;
        totalMass += stars[i].mass;
    }

    return momentOfMass/totalMass;
}

void Simulation::terminate() {
    // De-allocate Resources
    mesh.terminate();
    shader.destroy();
    computeShader.destroy();

    // Terminate GLFW
    window.terminate();
}
