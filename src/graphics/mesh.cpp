/** 
 * File: mesh.cpp
 * Description: Implementations for the Mesh class. 
*/

#define _USE_MATH_DEFINES
#include <math.h>
#include "mesh.h"

// Mesh Body Functions:

Mesh::MeshBody::MeshBody(const Body& body) {
    // Construct meshBody with data state from the body. Store
    // mass in the .w coordinate of the meshBody position. 
    this->position = glm::vec4(body.position, body.mass);
    this->velocity = glm::vec4(body.velocity, 0.0f);
    this->acceleration = glm::vec4(body.acceleration, 0.0f);
}


// Mesh Functions:

Mesh::Mesh(std::vector<float> vertices, std::vector<unsigned int> indices, int n) {
    this->vertices = vertices;
    this->indices = indices;
    this->n = n;

    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO); 
    glGenBuffers(1, &EBO); 
    glGenBuffers(1, &iVBO); 
    glGenBuffers(1, &SSBO);
    
    // Bind VAO
    glBindVertexArray(VAO);

    // Bind VBO buffer to vertices and configure attributes
    glBindBuffer(GL_ARRAY_BUFFER, VBO);  
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Bind EBO to indicies
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    
    // SSBO Instance Rendering Initilzation | Used only for GPU computation
    glBindBuffer(GL_ARRAY_BUFFER, SSBO);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);  
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(MeshBody), (void*)0);  
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    // iVBO Instance Rendering Initlization | Using only for CPU computation
    // glBindBuffer(GL_ARRAY_BUFFER, iVBO); 
    // glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);  
    // glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    // glEnableVertexAttribArray(2);
    // glVertexAttribDivisor(2, 1);

    // Bind SSBO
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, n * sizeof(MeshBody), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, SSBO);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0);
}

void Mesh::drawInstanced(std::vector<glm::vec3>& positions, Shader shader) {
    // Draw mesh in instanced positions obtained from the positions vector
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, iVBO);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), positions.data(), GL_DYNAMIC_DRAW);
    glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, positions.size());
    glBindVertexArray(0);
}

void Mesh::loadBodies(const std::vector<Body>& bodies) {
    // Conver the vector of bodies to a vector of meshBodies
    std::vector<MeshBody> meshBodies;
    meshBodies.reserve(bodies.size());

    for (const Body& body : bodies) {
        MeshBody mb(body);
        meshBodies.push_back(mb);
    }

    // Bind the SSBO to the GL SSBO type and load the position data from the SSBO 
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);   
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, meshBodies.size() * sizeof(MeshBody), meshBodies.data()); 
}

void Mesh::drawSSBO() {
    // Draw mesh in instanced positions from the SSBO
    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, n);
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

    for (int i = 0; i <= numVertices; i++) {
        indices.push_back(0);
        indices.push_back(i + 1);
        if (i < numVertices) { 
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
    // Delete Buffers
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &iVBO);
}