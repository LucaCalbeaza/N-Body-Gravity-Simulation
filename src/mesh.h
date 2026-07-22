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


class Mesh {
public:
    // Vertices and Indices Data
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // Buffers and Array
    unsigned int VBO, VAO, EBO;

    /**
     * Mesh Constructor: Creates a mesh with given vertices and 
     * indices. Sets up the OpenGL buffers and arrays.  
     */
    Mesh(std::vector<float> vertices, std::vector<unsigned int> indices);

    /**
     * Renders the mesh with the given model applied on 
     * the given shader
     */
    void draw(glm::mat4 model, Shader shader);

    /**
     * Sets the vertoces and indices of the mesh to represent 
     * a circle with the given radius, number of vertices and 
     * RGB color
     */
    void createCircle(float radius, unsigned int numVertices, float red, float green, float blue);

    /**
     * Deletes Buffers and arrays 
     */
    void terminate();

};

#endif