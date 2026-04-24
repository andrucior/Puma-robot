#pragma once

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../input/MouseController.h"

class Camera {
public:
    glm::vec3 cameraTarget = glm::vec3(0, 0, 0);
    glm::vec3 cameraPosition = glm::vec3(-2.0f, 1.0f, 2.0f);

    float yaw = -90.0f;   // obrót poziomy
    float pitch = 0.0f;   // obrót pionowy

    float moveSpeed = 1.0f;
    float sensitivity = 0.01f;

    glm::vec3 front;
    glm::vec3 right;
    glm::vec3 up;

    Camera() {
        updateVectors();
    }

    glm::mat4 view() const;

    void constrainPitch();
    
    void updateVectors();

    void update();

    void moveForward(float d);

    void moveRight(float d);

    void moveUp(float d);

    void updateFromController();
};
