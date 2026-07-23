/** 
 * File: mesh.cpp
 * Description: Implementations for the Mesh class. 
*/

#define _USE_MATH_DEFINES
#include <math.h>
#include "mesh.h"


Mesh::Mesh(std::vector<float> vertices, std::vector<unsigned int> indices) {
    this->vertices = vertices;
    this->indices = indices;

    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO); 
    glGenBuffers(1, &EBO); 
    
    // Bind VAO
    glBindVertexArray(VAO);

    // Bind VBO buffer to the vertices & EBO buffer to indices
    glBindBuffer(GL_ARRAY_BUFFER, VBO);  
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);  

    // Configure Vertex Positions Attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Configure Vertex Color Attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0);
}

void Mesh::draw(glm::mat4 model, Shader shader) {
    glBindVertexArray(VAO);
    unsigned int transformLoc = glGetUniformLocation(shader.ID, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::createCircle(float radius, unsigned int numVertices, float red, float green, float blue) {
    // Clear existing Mesh data
    vertices.clear();
    indices.clear();

    // Create circle origin
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);

    vertices.push_back(red);
    vertices.push_back(green);
    vertices.push_back(blue);

    // Create outer vertices
    for (int i = 0; i <= numVertices; i++) {
        float angle = 2.0f * M_PI * i / numVertices;
        float x = radius * cos(angle); 
        float y = radius * sin(angle);
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(0.0f);

        vertices.push_back(red);
        vertices.push_back(green);
        vertices.push_back(blue);
    }

    for (int i = 0; i <= 100; i++) {
        indices.push_back(0);
        indices.push_back(i + 1);
        if (i < 100) { 
            indices.push_back(i + 2);
        } else {
            indices.push_back(1);
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);  
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW); 
}

void Mesh::terminate() {
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &EBO);
}