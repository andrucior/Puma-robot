#define _USE_MATH_DEFINES

#include <iostream>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <vector>
#include <cmath>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <memory>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "KeyboardController.h"
#include "Camera.h"
#include "MouseController.h"
#include "puma/SceneShader.h"

GLFWwindow* initWindow(int& H, int& W);
void UpdateScreenSize(int& H, int& W);
bool handleResize(GLFWwindow* win, int& W, int& H, glm::mat4x4& P, GLuint tex);

int main() {
    int W, H;
    GLFWwindow* win;

    if ((win = initWindow(H, W)) == nullptr) return -1;

    std::shared_ptr<Camera> camera = std::make_shared<Camera>();
    KeyboardController keyboardController(*camera);

    glm::mat4x4 P = glm::perspective(60.0f * (float)(M_PI / 180.0), float(W) / H, 0.5f, 100.0f);
    
    // Quad VAO
    unsigned int VAO, VBO;
    
    float verts[] = {
        -1, 1, 0,1,  -1,-1, 0,0,  1,-1, 1,0,
        -1, 1, 0,1,   1,-1, 1,0,  1, 1, 1,1
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    

    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glViewport(0, 0, W, H);

    float lastTime = 0.0f;
    bool firstFrame = true;

    SceneShader shader = SceneShader(P, camera.get());

    while (!glfwWindowShouldClose(win)) {
        const float currentTime = (float)glfwGetTime();
        const float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        camera->update();
        
        if (MouseController::g_leftDown) {
            camera->updateFromController();
        }

        handleResize(win, W, H, P, tex);


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        shader.Draw();

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}

GLFWwindow* initWindow(int& H, int& W) {
    if (!glfwInit()) return nullptr;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    UpdateScreenSize(H, W);
    GLFWwindow* win = glfwCreateWindow(W, H, "Robot PUMA", nullptr, nullptr);
    if (!win) { glfwTerminate(); return nullptr; }

    glfwGetWindowSize(win, &W, &H);
    glfwMakeContextCurrent(win);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return nullptr;

    glfwSetMouseButtonCallback(win, MouseController::onSetMouseButtonCallback);
    glfwSetCursorPosCallback(win, MouseController::onSetCursorPosCallback);
    glfwSetScrollCallback(win, MouseController::onScrollCallback);
    glfwSetKeyCallback(win, InputManager::key_callback);

    return win;
}

void UpdateScreenSize(int& H, int& W) {
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    H = mode->height;
    W = mode->width;
}

bool handleResize(GLFWwindow* win, int& W, int& H, glm::mat4x4& P, GLuint tex) {
    int newW, newH;
    glfwGetFramebufferSize(win, &newW, &newH);
    if (newW == W && newH == H) return false;

    W = newW; H = newH;
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glViewport(0, 0, W, H);
    P = glm::perspective(60.0f * (float)(M_PI / 180.0), float(W) / H, 0.5f, 100.0f);
    return true;
}
