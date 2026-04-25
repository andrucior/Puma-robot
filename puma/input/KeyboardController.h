#pragma once

#include <glfw/glfw3.h>
#include <vector>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "../robot/PumaRobot.h"

class InputManager {
public:
	// Lista funkcji, które chcą słuchać o kliknięciach
	static inline std::vector<std::function<void(int, int)>> listeners;

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		for (auto& listener : listeners) {
			listener(key, action);
		}
	}
};

class KeyboardController {
private: 
	Camera& camera;
	PumaRobot& robot;
public:
    KeyboardController(Camera& camera, PumaRobot& robot) : camera(camera), robot(robot) {
		InputManager::listeners.push_back([this](int key, int action) {
			key_callback(key, action);
		});
	}

    void key_callback(int key, int action)
    {
        float speed = 0.05f;
		float angleSpeed = 1.0f;

        switch (key)
        {
        case GLFW_KEY_W:
            camera.moveForward(speed);
            break;

        case GLFW_KEY_S:
            camera.moveForward(-speed);
            break;

        case GLFW_KEY_A:
            camera.moveRight(-speed);
            break;

        case GLFW_KEY_D:
            camera.moveRight(speed);
            break;

        case GLFW_KEY_SPACE:
            camera.moveUp(speed);
            break;

        case GLFW_KEY_LEFT_SHIFT:
            camera.moveUp(-speed);
            break;

        // Sterowanie robotem
        case GLFW_KEY_R:
            robot.SetJointAngle(1, robot.GetJointAngle(1) + angleSpeed);
            break;
        case GLFW_KEY_F:
            robot.SetJointAngle(1, robot.GetJointAngle(1) - angleSpeed);
            break;

        case GLFW_KEY_T:
            robot.SetJointAngle(2, robot.GetJointAngle(2) + angleSpeed);
            break;
        case GLFW_KEY_G:
            robot.SetJointAngle(2, robot.GetJointAngle(2) - angleSpeed);
            break;

        case GLFW_KEY_Y:
            robot.SetJointAngle(3, robot.GetJointAngle(3) + angleSpeed);
            break;
        case GLFW_KEY_H:
            robot.SetJointAngle(3, robot.GetJointAngle(3) - angleSpeed);
            break;

        case GLFW_KEY_U:
            robot.SetJointAngle(4, robot.GetJointAngle(4) + angleSpeed);
            break;
        case GLFW_KEY_J:
            robot.SetJointAngle(4, robot.GetJointAngle(4) - angleSpeed);
            break;

        case GLFW_KEY_I:
            robot.SetJointAngle(5, robot.GetJointAngle(5) + angleSpeed);
            break;
        case GLFW_KEY_K:
            robot.SetJointAngle(5, robot.GetJointAngle(5) - angleSpeed);
            break;

        // Animacja
        case GLFW_KEY_C:
            if (action == GLFW_PRESS)
            {
                robot.isAnimating = !robot.isAnimating;
            }
            break;
        }
    }
};
