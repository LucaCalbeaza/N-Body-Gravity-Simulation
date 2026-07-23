/** 
 * File: simulation.h
 * Description: Declarations for the Simulation class. 
*/

#ifndef SIMULATION_H
#define SIMULATION_H

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

class Simulation {
public:
    // Graphic Properties
    Window window; 
    Shader shader;
    Mesh mesh;

    // Time Properties
    float dt;
    float frameTimeAccumulation;
    float lastFrameTime;

    // Physical Properties
    std::vector<Body> stars;
    unsigned int n;
    float mass;
    float G;
    const float minDistance = 0.05f;


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
     * calculating their gravitional interactions. 
     */
    void updatePhysics();

    /**
     * De-allocates resources and terminates the window
     */
    void terminate();
};

#endif