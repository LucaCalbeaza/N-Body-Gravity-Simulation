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

    // Time Properties
    float dt;
    float frameTimeAccumulation;
    float lastFrameTime;

    // Physical Properties
    std::vector<Body> stars;
    unsigned int n;
    float mass;
    float G;
    const float rSoft = 0.05f;


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
     * De-allocates resources and terminates the window
     */
    void terminate();
};

#endif