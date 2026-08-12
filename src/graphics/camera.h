/** 
 * File: camera.h
 * Description: Declarations for the Camera class. 
*/

#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera {
public:
    // Camera Attributes
    glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 worldUp = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 up;
    glm::vec3 right;
    
    // Orbit  Attributes 
    float radius = 5.0f;
    float minRadius = 0.5f;
    float maxRadius = 50.0f;
    
    // euler Angles
    float yaw = -45.0f;;
    float pitch = 15.0f;;
    // camera options
    float movementSpeed = 2.5f;
    float mouseSensitivity = 0.1f;
    float zoom = 45.0f;
    float zoomSensitivity = 0.5f;

    /**
     * Camera Constructor: Creates a camera object with default attributes.
     */
    Camera();

    /**
     * Returns the view matrix calculated using Euler Angles and the LookAt Matrix.
     */
    glm::mat4 GetViewMatrix();

    /**
     * Processes input received from any keyboard-like input system and update in
     * accordance with dt to account for fps inconsistencies.
     */
    void ProcessKeyboard(int key, float dt);

    /**
     * processes input received from a mouse input system. Expects the offset value
     * in both the x and y direction.
     */
    void ProcessMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch = true);

    /**
     * Processes input received from a mouse scroll-wheel event. Only requires 
     * input on the vertical wheel-axis.
     */
    void ProcessMouseScroll(float yOffset);

private:
    /**
     * Calculates the front vector from the Camera's (updated) Euler Angles
     */
    void updateCameraVectors();
};
#endif