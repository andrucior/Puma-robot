#include "Camera.h"

glm::mat4 Camera::view() const
{
    return glm::lookAt(
        cameraPosition,
        cameraPosition + front,
        up
    );
}

void Camera::constrainPitch()
{
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
}

void Camera::updateVectors()
{
    glm::vec3 f;
    f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    f.y = sin(glm::radians(pitch));
    f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    front = glm::normalize(f);

    right = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0)));
    up = glm::normalize(glm::cross(right, front));
}

void Camera::update()
{
    constrainPitch();
    updateVectors();
}

void Camera::moveForward(float d) 
{
    cameraPosition += front * d;
}

void Camera::moveRight(float d)
{
    cameraPosition += right * d;
}

void Camera::moveUp(float d)
{
    cameraPosition += glm::vec3(0, 1, 0) * d;
}

void Camera::updateFromController()
{
    yaw += MouseController::g_dx * sensitivity;
    pitch += MouseController::g_dy * sensitivity;
}