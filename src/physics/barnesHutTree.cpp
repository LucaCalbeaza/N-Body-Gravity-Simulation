/** 
 * File: barnes-hut-tree.cpp
 * Description: Declarations for the BarnesHutTree class. 
*/


#include "barnesHutTree.h"

// Node member functions:

BarnesHutTree::Node::Node(glm::vec3 centerPosition, float size) {
    this->centerPosition = centerPosition;
    this->size = size;
    centerOfMass = glm::vec3(0.0f,  0.0f,  0.0f);
    bodyCount = 0;
    totalMass = 0.0f;
    bodyIndex = -1;
}

bool BarnesHutTree::Node::isLeaf() {
    return childrenIndexes[0] == -1 && childrenIndexes[1] == -1 && childrenIndexes[2] == -1 && childrenIndexes[3] == -1;
}

// BarnesHutTree member functions:

BarnesHutTree::BarnesHutTree(float size, glm::vec3 centerOfMass, int n) {
    nodes.reserve(10 * n);
    addNode(centerOfMass, size);
    rootIndex = 0;
}

int BarnesHutTree::addNode(glm::vec3 centrePosition, float size) {
    nodes.push_back(Node(centrePosition, size));
    return nodes.size() - 1;
}

int BarnesHutTree::getQuadrant(glm::vec3 nodeCentre, glm::vec3 position) {
    int quadrant = 0;
    if (position.x >= nodeCentre.x) {
        quadrant += 1;
    }
    if (position.y >= nodeCentre.y) {
        quadrant += 2;
    }
    return quadrant;
}

glm::vec3 BarnesHutTree::getChildCentre(glm::vec3 parentCenter, float childSize, int quadrant) {
    glm::vec3 offSet;
    float halfQuadrant = childSize * 0.5;
    switch (quadrant) {
    case 0:
        offSet = glm::vec3(-halfQuadrant, -halfQuadrant, 0.0f);
        break;
    case 1:
        offSet = glm::vec3(halfQuadrant, -halfQuadrant, 0.0f);
        break;
    case 2:
        offSet = glm::vec3(-halfQuadrant, halfQuadrant, 0.0f);
        break;
    default:
        offSet = glm::vec3(halfQuadrant, halfQuadrant, 0.0f);
        break;
    }

    return parentCenter + offSet;
}

void BarnesHutTree::insert(int nodeIndex, int bodyIndex, std::vector<Body>& bodies) {
    const Body& body = bodies[bodyIndex];
    
    // Case 1: Node is an empty leaf
    if (nodes[nodeIndex].isLeaf() && nodes[nodeIndex].bodyIndex == -1) {
        nodes[nodeIndex].bodyIndex = bodyIndex;
        nodes[nodeIndex].centerOfMass = body.position;
        nodes[nodeIndex].totalMass = body.mass;
        nodes[nodeIndex].bodyCount = 1;
        return;
    }

    // Case 2: Node is an occupied leaf node
    if (nodes[nodeIndex].isLeaf() && nodes[nodeIndex].bodyIndex != -1) {
        Body oldBody = bodies[nodes[nodeIndex].bodyIndex];
        unsigned int oldBodyIndex = nodes[nodeIndex].bodyIndex;

        if (nodes[nodeIndex].size < 1e-4f) {
            nodes[nodeIndex].centerOfMass = computeCentreOfMass(body.position, nodes[nodeIndex].centerOfMass, body.mass, nodes[nodeIndex].totalMass);
            nodes[nodeIndex].totalMass += body.mass;
            nodes[nodeIndex].bodyCount++;
            return;
        }

        nodes[nodeIndex].bodyIndex = -1;
        unsigned int quadrant = getQuadrant(nodes[nodeIndex].centerPosition, oldBody.position);
        glm::vec3 childCenterPosition = getChildCentre(nodes[nodeIndex].centerPosition, nodes[nodeIndex].size * 0.5, quadrant); 
        nodes[nodeIndex].childrenIndexes[quadrant] = addNode(childCenterPosition, nodes[nodeIndex].size * 0.5);
        
        insert(nodes[nodeIndex].childrenIndexes[quadrant], oldBodyIndex, bodies);
        insert(nodeIndex, bodyIndex, bodies);
        return;
    }

    // Case 3: Node is an internal node 
    unsigned int quadrant = getQuadrant(nodes[nodeIndex].centerPosition, body.position);
    nodes[nodeIndex].centerOfMass = computeCentreOfMass(body.position, nodes[nodeIndex].centerOfMass, body.mass, nodes[nodeIndex].totalMass);
    nodes[nodeIndex].totalMass += body.mass;
    nodes[nodeIndex].bodyCount++;
    if (nodes[nodeIndex].childrenIndexes[quadrant] == -1) {
        glm::vec3 childCenterPosition = getChildCentre(nodes[nodeIndex].centerPosition, nodes[nodeIndex].size * 0.5, quadrant); 
        nodes[nodeIndex].childrenIndexes[quadrant] = addNode(childCenterPosition, nodes[nodeIndex].size * 0.5);
    }

    insert(nodes[nodeIndex].childrenIndexes[quadrant], bodyIndex, bodies);
}

glm::vec3 BarnesHutTree::computeAcceleration(int nodeIndex, int bodyIndex, std::vector<Body>& bodies, float theta, float G, float rSoft) {
    glm::vec3 acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    Node& node = nodes[nodeIndex]; 

    if (node.totalMass == 0) {
        return acceleration;
    }

    if (node.isLeaf() && node.bodyIndex == bodyIndex) {
        return acceleration;
    }

    glm::vec3 distance = bodies[bodyIndex].position - node.centerOfMass;

    if ((node.size / glm::length(distance)) < theta || node.isLeaf()) {
        float distanceSqr = glm::dot(distance, distance) + rSoft * rSoft;
        float distSixth = distanceSqr * distanceSqr * distanceSqr;
        float invDistCube = 1.0f / std::sqrt(distSixth); 
        acceleration -= G * node.totalMass * distance * invDistCube;
    } else {
        for (int i = 0; i < 4; i++) {
            if (node.childrenIndexes[i] != -1) {
                acceleration += computeAcceleration(node.childrenIndexes[i], bodyIndex, bodies, theta, G, rSoft);
            }
        }
    }

    return acceleration;
}

glm::vec3 BarnesHutTree::computeCentreOfMass(glm::vec3 position1, glm::vec3 position2, float mass1, float mass2) {
    float totalMass = mass1 + mass2;
    glm::vec3 momentOfMass = position1 * mass1 + position2 * mass2;
        
    return momentOfMass/totalMass;
}
