/** 
 * File: simulation.cpp
 * Description: Implementations for the Simulation class. 
*/

#include "simulation.h"
#include <iostream>
#include "omp.h"


Simulation::Simulation(unsigned int screenWidth, unsigned int screenHeight, float fps, unsigned int n, float mass, float G, float theta, bool is3D) :   
    window(screenWidth, screenHeight, "N-Body Orbital Simulation"),
    shader("src/shaders/vertexShader.glsl", "src/shaders/fragmentShader.glsl"),
    computeShader("src/shaders/computeShaderBruteForceTiled.glsl"),
    boundingBoxFirstStepShader(
        is3D ? "src/shaders/computeShaderOctTree/boundingBoxFirstStep3D.glsl"    : "src/shaders/computeShaderQuadTree/boundingBoxFirstStep2D.glsl",
        is3D ? "src/shaders/computeShaderOctTree/common3D.glsl"                  : "src/shaders/computeShaderQuadTree/common2D.glsl",
        1),
    boundingBoxSecondStepShader(
        is3D ? "src/shaders/computeShaderOctTree/boundingBoxSecondStep3D.glsl"   : "src/shaders/computeShaderQuadTree/boundingBoxSecondStep2D.glsl",
        is3D ? "src/shaders/computeShaderOctTree/common3D.glsl"                  : "src/shaders/computeShaderQuadTree/common2D.glsl",
        1),
    mortonCodeGenerationShader(
        is3D ? "src/shaders/computeShaderOctTree/mortonCodeGeneration3D.glsl"    : "src/shaders/computeShaderQuadTree/mortonCodeGeneration2D.glsl",
        is3D ? "src/shaders/computeShaderOctTree/common3D.glsl"                  : "src/shaders/computeShaderQuadTree/common2D.glsl",
        1),
    bitonicSortShader(
        is3D ? "src/shaders/computeShaderOctTree/bitonicSort3D.glsl"             : "src/shaders/computeShaderQuadTree/bitonicSort2D.glsl",
        is3D ? "src/shaders/computeShaderOctTree/common3D.glsl"                  : "src/shaders/computeShaderQuadTree/common2D.glsl",
        1),
    quadTreeBuildShader(
        is3D ? "src/shaders/computeShaderOctTree/octTreeBuild3D.glsl"            : "src/shaders/computeShaderQuadTree/quadTreeBuild2D.glsl",
        is3D ? "src/shaders/computeShaderOctTree/common3D.glsl"                  : "src/shaders/computeShaderQuadTree/common2D.glsl",
        1),
    centerOfMassReductionShader(
        is3D ? "src/shaders/computeShaderOctTree/centerOfMassReduction3D.glsl"   : "src/shaders/computeShaderQuadTree/centerOfMassReduction2D.glsl",
        is3D ? "src/shaders/computeShaderOctTree/common3D.glsl"                  : "src/shaders/computeShaderQuadTree/common2D.glsl",
        1),
    accelerationComputationShader(
        is3D ? "src/shaders/computeShaderOctTree/accelerationComputation3D.glsl" : "src/shaders/computeShaderQuadTree/accelerationComputation2D.glsl",
        is3D ? "src/shaders/computeShaderOctTree/common3D.glsl"                  : "src/shaders/computeShaderQuadTree/common2D.glsl",
        1),
    mesh(std::vector<float>(), std::vector<unsigned int>(), n),
    n(n),
    mass(mass),
    G(G),
    screenSize(screenWidth),               
    theta(theta),
    is3D(is3D)            {
    
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
    mesh.loadBodies(stars);
    mesh.initBarnesHutTree(is3D);

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
        glm::vec3 position = glm::vec3(genRandom(gen),  genRandom(gen),  genRandom(gen));
        glm::vec3 veloctiy = glm::vec3(0.1*genRandom(gen),  0.1*genRandom(gen),  0.1*genRandom(gen));
        glm::vec3 acceleration = glm::vec3(0.0f,  0.0f,  0.0f);
        Body star(position, veloctiy, acceleration, mass/n);
        stars.push_back(star);
    }
}

Mesh Simulation::generateMesh() {
    // Create circle mesh and load the star data into the mesh SSBO
    //mesh.createCircle(0.001f, 10, 1.0f, 1.0f, 1.0f);
    mesh.createSphere(0.002f, 1.0f, 1.0f, 1.0f);
    return mesh;
}

void Simulation::run() {
    // While loop runs while the window remains open
    while(!glfwWindowShouldClose(window.window)) {
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

        // Register Input
        window.processInput(frameTime);

        // Update the star data. Incase of frame drops update the star data 
        // multiple times to account for loss. Limit set to maxStepsPerFrame
        // to prevent the frame drop from spiraling out of control. 
        int steps = 0;
        while (frameTimeAccumulation >= dt && steps < maxStepsPerFrame) {
            updatePhysicsBarnesHutTreeComputeShader(theta);
            //updatePhysicsBruteForceComputeShader();
            frameTimeAccumulation -= dt;
            steps++;
            if (steps == maxStepsPerFrame) {
                frameTimeAccumulation = 0;
            }
        }

        // Render and active shader program
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();

        // pass projection matrix to shader 
        glm::mat4 projection = glm::perspective(glm::radians(window.camera.zoom), 1.0f, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, &projection[0][0]);

        // pass view matrix to shader
        glm::mat4 view = window.camera.GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, &view[0][0]);

        // Draw
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

void Simulation::updatePhysicsBruteForceComputeShader() {
    // Active the compute shader
    computeShader.use();

    // Initialize the computer shader with the uniform values 
    // of the given physcial terms: n, G, rSoft, dt
    glUniform1i(glGetUniformLocation(computeShader.ID, "n"), n);
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

void Simulation::updatePhysicsBarnesHutTreeComputeShader(float theta) {
    unsigned int numGroups = (n + 255) / 256;

    // Bounding Box First Step
    boundingBoxFirstStepShader.use();
    glUniform1i(glGetUniformLocation(boundingBoxFirstStepShader.ID, "n"), n);
    glDispatchCompute(numGroups, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    // Bounding Box Second Step
    boundingBoxSecondStepShader.use();
    glUniform1i(glGetUniformLocation(boundingBoxSecondStepShader.ID, "numGroups"), numGroups);
    glDispatchCompute(1, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);    

    // Compoute totalSize and padded groups
    unsigned int totalSize = 1;
    while (totalSize < n) {
        totalSize <<= 1;
    }
    unsigned int numGroupsPadded = (totalSize + 255) / 256;

    // Generate Morton Code
    mortonCodeGenerationShader.use();
    glUniform1i(glGetUniformLocation(mortonCodeGenerationShader.ID, "n"), n);
    glUniform1ui(glGetUniformLocation(mortonCodeGenerationShader.ID, "totalSize"), totalSize);
    glDispatchCompute(numGroupsPadded, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);   

    // Bitonic Sort
    bitonicSortShader.use();
    glUniform1i(glGetUniformLocation(bitonicSortShader.ID, "n"), n);
    GLint blockSizeLocation = glGetUniformLocation(bitonicSortShader.ID, "blockSize");
    GLint stepSizeLocation = glGetUniformLocation(bitonicSortShader.ID, "stepSize");
    GLint totalSizeLocation = glGetUniformLocation(bitonicSortShader.ID, "totalSize");
    
    glUniform1ui(totalSizeLocation, totalSize);
    for (unsigned int blockSize = 2; blockSize <= totalSize; blockSize <<= 1) {
        for (unsigned int stepSize = blockSize >> 1; stepSize > 0; stepSize >>= 1) {
            glUniform1ui(blockSizeLocation, blockSize);
            glUniform1ui(stepSizeLocation, stepSize);
            glDispatchCompute(numGroupsPadded, 1, 1);
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT); 
        }
    }

    // Initialize tree root Node: 
    if (is3D) {
        float sceneBounds[8];
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, mesh.scenceBoundsBuf);
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(sceneBounds), sceneBounds);
        mesh.resetBarnesHutTree(sceneBounds, is3D);
    } else {
        float sceneBounds[4];
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, mesh.scenceBoundsBuf);
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(sceneBounds), sceneBounds);
        mesh.resetBarnesHutTree(sceneBounds, is3D);
    }


    // Quad Tree Build
    quadTreeBuildShader.use();
    GLint levelLoc = glGetUniformLocation(quadTreeBuildShader.ID, "level");
    int maxLevel = is3D ? 21 : 16; 
    GLuint activeBuffer = mesh.activeABuf;
    GLuint nextBuffer = mesh.activeBBuf;
    
    for (int level = 0; level < maxLevel; level++) {
        // Bind buffers
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, activeBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, nextBuffer);

        // Set the uniform value, compute the number of GPU workgroups 
        // needed and than compute. 
        glUniform1i(levelLoc, level);
        unsigned int numGroupsLevel = (n + 63) / 64;
        glDispatchCompute(numGroupsLevel, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        // Find the number of nodes on the current level and the next level
        uint32_t counts[2];
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, mesh.activeCountsBuf);
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(counts), counts);
        uint32_t countOut = counts[1];
        // If there are no nodes on the next level exit the loop
        if (countOut == 0) {
            break;
        }

        // Swap buffers and prepare for next level
        std::swap(activeBuffer, nextBuffer);
        uint32_t nextCounts[2] = {countOut, 0u};
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(nextCounts), nextCounts);
    }

    // Reset Buffer A as the default active buffer
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, mesh.activeABuf);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, mesh.activeBBuf);

    // Center of Mass Reduction
    centerOfMassReductionShader.use();
    glUniform1i(glGetUniformLocation(centerOfMassReductionShader.ID, "n"), n);
    glDispatchCompute(numGroups, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


    // Acceleration Computation & Body Update
    accelerationComputationShader.use();
    glUniform1i(glGetUniformLocation(accelerationComputationShader.ID, "n"), n);
    glUniform1i(glGetUniformLocation(accelerationComputationShader.ID, "rootIndex"), 0);
    glUniform1f(glGetUniformLocation(accelerationComputationShader.ID, "theta"), theta);
    glUniform1f(glGetUniformLocation(accelerationComputationShader.ID, "G"), G);
    glUniform1f(glGetUniformLocation(accelerationComputationShader.ID, "rSoft"), rSoft);
    glUniform1f(glGetUniformLocation(accelerationComputationShader.ID, "dt"), dt);
    glDispatchCompute(numGroups, 1, 1);
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
