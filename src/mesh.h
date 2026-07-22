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
    // Buffers and Array
    unsigned int VBO, VAO, EBO;
    unsigned int verticesCount;

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
     * Deletes Buffers and arrays 
     */
    void terminate();

};

#endif