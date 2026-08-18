/** 
 * File: camera.cpp
 * Description: Implementations for the Camera class. Modified code 
 * from https://learnopengl.com/Getting-started/Camera  
*/

#include "camera.h"
#include <iostream>

Camera::Camera(bool window3D) {
    setDefault(window3D);
}

void Camera::setDefault(bool window3D) {
    target = glm::vec3(0.0f, 0.0f, 0.0f);
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    if (window3D) {
        radius = 4.0f;
        yaw = -45.0f;
        pitch = 15.0f;
    } else {
        radius = 2.5f;
        yaw = 0.0f;
        pitch = 90.0f;
    }
    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() {
    return glm::lookAt(position, target, up);
}

void Camera::ProcessKeyboard(int key, float dt, bool window3D) {
    float velocity = movementSpeed * dt;
    switch (key) {
        // W Key
        case 0:
            target += up * velocity;
            break;
        // S Key
        case 1:
            target -= up * velocity;
            break;
        // A Key
        case 2: 
            target -= right * velocity;
            break;
        // D Key
        case 3:
            target += right * velocity;
            break;
        // Spacebar
        default: 
            setDefault(window3D);
            break;
    }
    updateCameraVectors();
}

void Camera::ProcessMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch) {
    xOffset *= mouseSensitivity;
    yOffset *= mouseSensitivity;

    yaw -= xOffset;
    pitch -= yOffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped.
    if (constrainPitch) {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }
    updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yOffset) {
    radius -= yOffset * zoomSensitivity;
    if (radius < minRadius) radius = minRadius;
    if (radius > maxRadius) radius = maxRadius;
    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    // Convert Spherical Coord to Cartesian offset & 
    // and then add to the position.
    glm::vec3 offset;
    offset.x = radius * cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    offset.y = radius * cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    offset.z = radius * sin(glm::radians(pitch));
    position = target + offset;

    // front/right/up are now derived from position and target rather
    // than driving position themselves.
    front = glm::normalize(target - position);
    right = glm::normalize(glm::cross(front, worldUp));
    up    = glm::normalize(glm::cross(right, front));
}


