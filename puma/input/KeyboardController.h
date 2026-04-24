#pragma once

#include <glfw/glfw3.h>
#include <vector>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"

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
public:
	KeyboardController(Camera& camera) : camera(camera) {
		InputManager::listeners.push_back([this](int key, int action) {
			key_callback(key, action);
		});
	}

    void key_callback(int key, int action)
    {
        float speed = 0.05f;

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
        }
    }
};
