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
    // ------------ Mesh Sturctures --------------
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

    // GpuNode structure on the CPU. Mimics the field layout of the 
    // GPU structure exactly. 
    struct GpuNodeCPU {
        float comAndMass[4];
        float centerAndSize[4];
        int32_t children[4];
        int32_t bodyIndex;
        int32_t rangeStart;
        int32_t rangeEnd;
        int32_t checkInCount;
        int32_t childCount;
        int32_t _pad0, _pad1, _pad2;
    };


    // ------------ Mesh --------------------


    // Vertices and Indices Data
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    int n;

    // Standard Buffers and Arrays
    GLuint VBO, VAO, EBO, iVBO, SSBO;

    // Barnes-Hut Compute Shader Buffers & int variables;
    GLuint sortedIdxBuf, mortonCodeBuf, nodesBuf, nextFreeNodeBuf, scenceBoundsBuf;
    GLuint partialBoundsBuf, activeABuf, activeBBuf, activeCountsBuf, leafNodeBuf, nodeParentBuf;
    unsigned int totalSizePadded, numGroupsPadded, maxNodes;

    /**
     * Mesh Constructor: Creates a mesh with given vertices and 
     * indices. Sets up the OpenGL buffers and arrays.  
     */
    Mesh(std::vector<float> vertices, std::vector<unsigned int> indices, int n);

    /**
     * Returns an SSBO object ID with the given size and data attached 
     * to the given binding
     */
    GLuint makeSSBO(size_t byteSize, const void* data, GLuint bindingLocation);

    /**
     * Loads the given bodies into the SSBO 
     */
    void loadBodies(const std::vector<Body>& bodies);

    /**
     * Initializes the Barnes-Hut Tree variables and all of the 
     * SSBO buffers.
     */
    void initBarnesHutTree();

    /**
     * Resets the Barnes-Hut root node and also resets the 
     * buffer data for several of the buffers. 
     */
    void resetBarnesHutTree(const float sceneBounds[4]);

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
     * Sets the vertoces and indices of the mesh to represent 
     * an icosphere of 12 vertices with the given radius, and RGB color.
     */
    void createSphere(float radius, float red, float green, float blue);

    /**
     * Deletes Buffers and arrays.
     */
    void terminate();

};

#endif