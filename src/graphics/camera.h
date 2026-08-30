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
    glm::vec3 target;
    glm::vec3 position;
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 worldUp = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 up;
    glm::vec3 right;
    
    // Orbit  Attributes 
    float radius;
    float minRadius = 0.5f;
    float maxRadius = 50.0f;
    
    // euler Angles
    float yaw;
    float pitch;
    // camera options
    float movementSpeed = 2.5f;
    float mouseSensitivity = 0.1f;
    float zoom = 45.0f;
    float zoomSensitivity = 0.5f;

    /**
     * Camera Constructor: Creates a camera object. If the window is 3D the 
     * default camera has yaw = -45.0f & pitch = 15.0f. If the window is not 
     * 3D the default camera is placed directly above with yaw = 0.0f & 
     * pitch = 90.0f.
     */
    Camera(bool window3D);

    /**
     * Set Camera to default placement
     */
    void setDefault(bool window3D);

    /**
     * Returns the view matrix calculated using Euler Angles and the LookAt Matrix.
     */
    glm::mat4 GetViewMatrix();

    /**
     * Processes input received from any keyboard-like input system and update in
     * accordance with dt to account for fps inconsistencies.
     */
    void ProcessKeyboard(int key, float dt, bool window3D);

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

    /**
     * Calculates the front vector from the Camera's (updated) Euler Angles
     */
    void updateCameraVectors();
};
#endif