/** 
 * File: body.h
 * Description: Declarations for the Body class. 
*/

#ifndef BODY_H
#define BODY_H

#include <iostream>
#include <string>
#include <vector>
#include <glm/glm.hpp>

class Body {
public:
    // Body Properties
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    float mass;

    /**
     * Body Constructor: Creates a body with given position, velocity, 
     * acceleration and mass.  
     */
    Body(glm::vec3 position, glm::vec3 velocity, glm::vec3 acceleration, float mass);

    /**
     * Updates position and velocity over the given time interval dt. 
     */
    void update(float dt);

};

#endif