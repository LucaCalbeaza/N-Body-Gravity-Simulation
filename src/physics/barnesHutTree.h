/** 
 * File: barnes-hut-tree.h
 * Description: Declarations for the BarnesHutTree class. 
*/

#ifndef BARNESHUTTREE_H
#define BARNESHUTTREE_H

#include <string>
#include "body.h"
#include <glm/glm.hpp>

class BarnesHutTree {
public:
    struct Node {
        glm::vec3 centerPosition;
        glm::vec3 centerOfMass;
        float size; 
        float totalMass;
        unsigned int bodyCount;
        int bodyIndex;
        int childrenIndexes[4] = {-1, -1, -1, -1};

        /**
         * Node Constructor: Constructs node with given 
         * center and size. Mass, index and children are 
         * set to 0 or empty.
         */
        Node(glm::vec3 centerPosition, float size);

        /**
         * Return true if the node has no children.
         */
        bool isLeaf();
    };

    // Barnes-Hut Tree Variables
    std::vector<Node> nodes;
    unsigned int rootIndex;

    /**
     * Barnes-Hut Tree Constructor: Constructs an empty tree with
     * the root node as an empty leaf node with size of the screen.
     */
    BarnesHutTree(float screenSize);

    /**
     * Adds a new node to the end of the array with the 
     * given centre and size. Returns the index of the new 
     * node.
     */
    int addNode(glm::vec3 centerPosition, float size);

    /**
     * Return the designated quadrant for the given position 
     * relative to the given node centre.
     * NE = 3, NW = 2, SW = 1, SE = 0
     */
    int getQuadrant(glm::vec3 nodeCenter, glm::vec3 position);

    /**
     * Return the position of the centre of a child node in the given 
     * quadrant.
     */
    glm::vec3 getChildCentre(glm::vec3 parentCenter, float childSize, int quadrant);

    /**
     * Insert the body at the given index into the tree. 
     * --- Three Cases --- 
     * Case 1: Node at the nodeIndex is an empty leaf node. 
     * Result: the node now stores the given bodyIndex. 
     * 
     * Case 2: Node at the nodeIndex is a leaf node that already 
     * stores a valid bodyIndex. 
     * Result: The current node is turned into an internal node 
     * with both the original body and the current body being passed 
     * to the node's children
     * 
     * Case 3: Node at the nodeIndex is an internal node. 
     * Result: The current body is inserted into the child node in 
     * the appropriate quadrant. If the chosen child node does not 
     * exist a new child node is created which stores the 
     * current body.  
     */
    void insert(int nodeIndex, int bodyIndex, std::vector<Body>& bodies);

    /**
     * Compute the acceleration as a result of the gravitational 
     * interaction from the other bodies onto the body at the 
     * given index using the given theta threshold. 
     */
    glm::vec3 computeAcceleration(int nodeIndex, int bodyIndex, std::vector<Body>& bodies, float theta, float G, float rSoft);

    /**
     * Given the mass and position of the 2 bodies, return 
     * the centre of mass of the system.
     */
    glm::vec3 computeCentreOfMass(glm::vec3 position1, glm::vec3 position2, float mass1, float mass2);
};

#endif