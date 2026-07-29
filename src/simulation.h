/** 
 * File: simulation.h
 * Description: Declarations for the Simulation class. 
*/

#ifndef SIMULATION_H
#define SIMULATION_H

#include <random>
#include <cmath>

#define _USE_MATH_DEFINES
#include <math.h>

#include "graphics/shader.h"
#include "graphics/window.h"
#include "graphics/mesh.h"
#include "physics/body.h"
#include "physics/barnesHutTree.h"

class Simulation {
public:
    // Graphic Properties
    Window window; 
    Shader shader;
    Mesh mesh;
    unsigned int screenSize;

    // Time and Frame Count Properties
    float dt;
    float frameTimeAccumulation = 0.0;
    float lastFrameTime;
    const int maxStepsPerFrame = 4;
    int fpsFrameCount = 0;
    float fpsElapsedTime = 0.0f;
    float currentFPS = 0.0f;

    // Physical Properties
    std::vector<Body> stars;
    std::vector<int> innerBodies;
    std::vector<int> outerBodies;
    std::vector<glm::vec3> positions;
    unsigned int n;
    float mass;
    float G;
    const float rSoft = 0.05f;
    float boundaryRadius = 2.0f;


    /**
     * Simulation Constructor: Initializes and runs the simulation 
     * at the given screen dimensions at the given fps, with the 
     * given physical properties.
     */
    Simulation(unsigned int screenWidth, unsigned int screenHeight, float fps, unsigned int n, float mass, float G);

private:
    /**
     * Creates the mesh(es) for the simulation.
     */
    Mesh generateMesh();

    /**
     * Adds n random stars to with randomized initial positions and 
     * initial velocity to the simulation.
     */
    void generateStarData();

    /**
     * Runs the simulation updating and drawing the stars on 
     * time interval dt. Ends the simulation when the window is closed and 
     * de-allocates resources afterwards.
     */
    void run();

    /**
     * Updates the acceleration, velocity and position of each star by 
     * calculating their gravitional interactions via a brute force 
     * summation method.  
     */
    void updatePhysicsBruteForce();

    /**
     * Updates the acceleration, velocity and position of each star by 
     * calculating their gravitional interactions via a Barnes-Hut tree
     * computation. 
     */
    void updatePhysicsBarnesHutTree();

    /**
     * Return the center of mass of the stars in the system 
     */
    glm::vec3 computeCenterOfMass();

    /**
     * De-allocates resources and terminates the window
     */
    void terminate();
};

#endif