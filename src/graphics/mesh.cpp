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
    glGenVertexArrays(1, &pointVAO); 
    
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
    
    // Set up Instanced Position Attribute 
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(MeshBody), (void*)0);  
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    // Set up Instanced Veloctiy Attribute
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(MeshBody), (void*)(4 * sizeof(float)));  
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

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

GLuint Mesh::makeSSBO(size_t byteSize, const void* data, GLuint bindingLocation) {
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, byteSize, data, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingLocation, buffer);
    return buffer;
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

void Mesh::initBarnesHutTree(bool is3D) {
    // Find the total padded size needed (first 2^i < n), and subsequently 
    // the number of GPU thread groups needed. 
    totalSizePadded = 1; 
    while (totalSizePadded < n) {
        totalSizePadded <<= 1;
    }
    numGroupsPadded = (totalSizePadded + 255) / 256;

    // Provide extra headroom for the max number of nodes. n * 2-4 should 
    // typically be enough, so choose 8 to be safe. 
    maxNodes = n * 8;
    
    // Create and bind SSBO ID object to buffer variables. Buffer for the 
    // bodies buffer was already bound in the loadBodies() function. 
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, SSBO);
    sortedIdxBuf = makeSSBO(totalSizePadded * sizeof(uint32_t), nullptr, 1);
    nextFreeNodeBuf = makeSSBO(sizeof(uint32_t), nullptr, 4);
    activeABuf = makeSSBO(maxNodes * sizeof(int32_t), nullptr, 7);
    activeBBuf = makeSSBO(maxNodes * sizeof(int32_t), nullptr, 8);
    activeCountsBuf = makeSSBO(sizeof(uint32_t) * 2, nullptr, 9);
    leafNodeBuf = makeSSBO(n * sizeof(int32_t), nullptr, 10);
    nodeParentBuf = makeSSBO(maxNodes * sizeof(int32_t), nullptr, 11);

    if (is3D) {
        mortonCodeBuf = makeSSBO(n * sizeof(uint64_t), nullptr, 2);
        nodesBuf = makeSSBO(maxNodes * sizeof(GpuNodeCPU3D), nullptr, 3);
        scenceBoundsBuf = makeSSBO(sizeof(float) * 4 * 2, nullptr, 5);
        partialBoundsBuf = makeSSBO(((n + 255) / 256) * sizeof(float) * 4 * 2, nullptr, 6);
    } else {
        mortonCodeBuf = makeSSBO(n * sizeof(uint32_t), nullptr, 2);
        nodesBuf = makeSSBO(maxNodes * sizeof(GpuNodeCPU2D), nullptr, 3);
        scenceBoundsBuf = makeSSBO(sizeof(float) * 4, nullptr, 5);
        partialBoundsBuf = makeSSBO(((n + 255) / 256) * sizeof(float) * 4, nullptr, 6);
    }
}

void Mesh::resetBarnesHutTree(const float sceneBounds[8], bool is3D) {
    // Create the root node with the given sceneBounds, the center
    // of mass and mass are initialized to 0 before any bodies are added.
    GpuNodeCPU2D root2D{};
    GpuNodeCPU3D root3D{};
    
    if (is3D) {
        // Set root node (x,y,z) positions and size for 3D case
        root3D.centerAndSize[0] = (sceneBounds[0] + sceneBounds[4]) * 0.5f; 
        root3D.centerAndSize[1] = (sceneBounds[1] + sceneBounds[5]) * 0.5f; 
        root3D.centerAndSize[2] = (sceneBounds[2] + sceneBounds[6]) * 0.5f;
        float sizeX = sceneBounds[4] - sceneBounds[0];
        float sizeY = sceneBounds[5] - sceneBounds[1];
        float sizeZ = sceneBounds[6] - sceneBounds[2];
        root3D.centerAndSize[3] = std::max({sizeX, sizeY, sizeZ}) * 1.01f;

        root3D.bodyIndex = -1;
        root3D.rangeStart = 0;
        root3D.rangeEnd = n;
        root3D.checkInCount = 0;
        root3D.childCount = 0;

        // Reset buffers
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, nodesBuf);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GpuNodeCPU3D), &root3D);
    } else {
        // Set root node (x,y,z) positions and size for 2D case
        root2D.centerAndSize[0] = (sceneBounds[0] + sceneBounds[2]) * 0.5f; 
        root2D.centerAndSize[1] = (sceneBounds[1] + sceneBounds[3]) * 0.5f; 
        root2D.centerAndSize[2] = 0.0f; 
        float sizeX = sceneBounds[2] - sceneBounds[0];
        float sizeY = sceneBounds[3] - sceneBounds[1];
        root2D.centerAndSize[3] = std::max(sizeX, sizeY) * 1.01f;

        root2D.bodyIndex = -1;
        root2D.rangeStart = 0;
        root2D.rangeEnd = n;
        root2D.checkInCount = 0;
        root2D.childCount = 0;

        // Reset buffers
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, nodesBuf);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GpuNodeCPU2D), &root2D);
    }

    uint32_t nextFreeNodeInit = 1; 
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, nextFreeNodeBuf);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(uint32_t), &nextFreeNodeInit);

    int32_t nodeParentRoot = -1;
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, nodeParentBuf);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(int32_t), &nodeParentRoot);

    int32_t rootActive = 0;
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, activeABuf);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(int32_t), &rootActive);

    uint32_t countsInit[2] = {1u, 0u}; 
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, activeCountsBuf);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(countsInit), countsInit);
}

void Mesh::drawInstanced(std::vector<glm::vec3>& positions, Shader shader) {
    // Draw mesh in instanced positions obtained from the positions vector
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, iVBO);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), positions.data(), GL_DYNAMIC_DRAW);
    glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, positions.size());
    glBindVertexArray(0);
}

void Mesh::drawSSBOMesh() {
    // Draw mesh in instanced positions from the SSBO
    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, n);
    glBindVertexArray(0);
}

void Mesh::drawSSBOPoints() {
    glBindVertexArray(pointVAO);
    glEnable(GL_PROGRAM_POINT_SIZE);       

    // Bind Bodies 
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, SSBO);

    // Enable additive blend
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);            
    glDepthMask(GL_FALSE);                        

    // Draw points
    glDrawArrays(GL_POINTS, 0, n);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
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

void Mesh::createSphere(float radius, float red, float green, float blue) {
    // Clear existing Mesh data
    vertices.clear();
    indices.clear();

    // Find Golden Ratio
    float goldenRatio = (1.0f + std::sqrt(5.0f)) / 2.0f;

    // Create Default Vertices
    std::vector<glm::vec3> baseVertices = {
        glm::vec3(-1, goldenRatio, 0), glm::vec3(1, goldenRatio, 0), glm::vec3(-1, -goldenRatio, 0), glm::vec3(1, -goldenRatio, 0), 
        glm::vec3(0, -1, goldenRatio), glm::vec3(0, 1, goldenRatio), glm::vec3(0, -1, -goldenRatio), glm::vec3(0, 1, -goldenRatio),
        glm::vec3(goldenRatio, 0, -1), glm::vec3(goldenRatio, 0, 1), glm::vec3(-goldenRatio, 0, -1), glm::vec3(-goldenRatio, 0, 1)
    };

    // Normalize and Adjust for Radius
    for (const auto& vertex : baseVertices) {
        float length = std::sqrt(vertex.x * vertex.x + vertex.y * vertex.y + vertex.z * vertex.z);
        vertices.push_back((vertex.x / length) * radius);
        vertices.push_back((vertex.y / length) * radius);
        vertices.push_back((vertex.z / length) * radius);

        vertices.push_back(red);
        vertices.push_back(green);
        vertices.push_back(blue);
    }

    // Create Indices
    indices = {
        0, 11, 5,   0, 5, 1,   0, 1, 7,   0, 7, 10,  0, 10, 11, 
        1, 5, 9,    5, 11, 4,  11, 10, 2, 10, 7, 6,  7, 1, 8,   
        3, 9, 4,    3, 4, 2,   3, 2, 6,   3, 6, 8,   3, 8, 9,   
        4, 9, 5,    2, 4, 11,  6, 2, 10,  8, 6, 7,   9, 8, 1   
    };

    glBindBuffer(GL_ARRAY_BUFFER, VBO);  
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW); 
}

void Mesh::terminate() {
    // Delete Standard Buffers
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &iVBO);
    glDeleteBuffers(1, &SSBO);
    glDeleteVertexArrays(1, &pointVAO);

    // Delete Barnes-Hut Compute Shader Buffers
    glDeleteBuffers(1, &sortedIdxBuf);
    glDeleteBuffers(1, &mortonCodeBuf);
    glDeleteBuffers(1, &nodesBuf);
    glDeleteBuffers(1, &nextFreeNodeBuf);
    glDeleteBuffers(1, &scenceBoundsBuf);
    glDeleteBuffers(1, &partialBoundsBuf);
    glDeleteBuffers(1, &activeABuf);
    glDeleteBuffers(1, &activeBBuf);
    glDeleteBuffers(1, &activeCountsBuf);
    glDeleteBuffers(1, &leafNodeBuf);
    glDeleteBuffers(1, &nodeParentBuf);
}