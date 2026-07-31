/** 
 * File: mesh.h
 * Description: Declarations for the Mesh class. 
*/

#ifndef MESH_H
#define MESH_H

#include <iostream>
#include <string>
#include <vector>
#include "shader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../physics/body.h"


class Mesh {
public:
    struct MeshBody {
        // Mass stored in position.w
        glm::vec4 position;          
        glm::vec4 velocity;        
        glm::vec4 acceleration;

        /**
         * Mesh Body constructor: assigns the position, 
         * velocity, and acceleration from the given body. 
         * Assigns the mass of the body to the w coordinate 
         * of the position. 
         */
        MeshBody(const Body& body);
    };


    // Vertices and Indices Data
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    int n;

    // Buffers and Array
    unsigned int VBO, VAO, EBO, iVBO, SSBO;

    /**
     * Mesh Constructor: Creates a mesh with given vertices and 
     * indices. Sets up the OpenGL buffers and arrays.  
     */
    Mesh(std::vector<float> vertices, std::vector<unsigned int> indices, int n);

    /**
     * Loads the given bodies into the SSBO 
     */
    void loadBodies(const std::vector<Body>& bodies);

    /**
     * Renders the given mesh at each of the given positions
     */
    void drawInstanced(std::vector<glm::vec3>& positions, Shader shader);

    /**
     * Renders the mesh at the positions given in the SSBO
     */
    void drawSSBO();

    /**
     * Sets the vertoces and indices of the mesh to represent 
     * a circle with the given radius, number of vertices and 
     * RGB color.
     */
    void createCircle(float radius, unsigned int numVertices, float red, float green, float blue);

    /**
     * Deletes Buffers and arrays.
     */
    void terminate();

};

#endif