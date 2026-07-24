/** 
 * File: body.cpp
 * Description: Implementations for the Body class. 
*/

#include "body.h"

Body::Body(glm::vec3 position, glm::vec3 velocity, glm::vec3 acceleration, float mass) {
    this->position = position;
    this->velocity = velocity;
    this->acceleration = acceleration;
    this->mass = mass;
}

void Body::update(float dt) {
    velocity += acceleration * dt; 
    position += velocity * dt;
    acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
}

