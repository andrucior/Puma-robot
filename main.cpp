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
#include "puma/Room.h"
#include "puma/robot/PumaRobot.h"
#include "puma/ShadowVolumeShader.h"

GLFWwindow* initWindow(int& H, int& W);
void UpdateScreenSize(int& H, int& W);
bool handleResize(GLFWwindow* win, int& W, int& H, glm::mat4x4& P, GLuint tex);

int main() {
    int W, H;
    GLFWwindow* win;

    if ((win = initWindow(H, W)) == nullptr) return -1;

    std::shared_ptr<Camera> camera = std::make_shared<Camera>();
    std::shared_ptr<PumaRobot> pumaRobot = std::make_shared<PumaRobot>();

    KeyboardController keyboardController(*camera, *pumaRobot);

    glm::mat4x4 P = glm::perspective(60.0f * (float)(M_PI / 180.0), float(W) / H, 0.5f, 100.0f);

    // Animation 
    float animationAngle = 0.0f;
    float animationSpeed = 1.5f;
    float circleRadius = 0.4f;
    glm::vec3 circleCenter = glm::vec3(-1.6f, 0.3f, 0.0f);

    float tiltAngle = -60;
    glm::vec3 tiltAxis = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 targetNormal = glm::vec3(0.0f, 1.0f, 0.0f);

    // Quad VAOd
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

    glEnable(GL_STENCIL_TEST);
    glClearStencil(0);

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glViewport(0, 0, W, H);

    float lastTime = 0.0f;
    bool firstFrame = true;

    SceneShader shader(P, camera.get());
    ShadowVolumeShader shadowShader(P, camera.get());
    Room room;
    const char* quadVert = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    void main() { gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); }
)";
    const char* quadFrag = R"(
    #version 330 core
    out vec4 FragColor;
    void main() { FragColor = vec4(0.0, 0.0, 0.0, 0.5); } // Półprzezroczysty czarny cień
)";

    GLuint vQuadShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vQuadShader, 1, &quadVert, nullptr);
    glCompileShader(vQuadShader);

    GLuint fQuadShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fQuadShader, 1, &quadFrag, nullptr);
    glCompileShader(fQuadShader);

    GLuint quadShader = glCreateProgram();
    glAttachShader(quadShader, vQuadShader);
    glAttachShader(quadShader, fQuadShader);
    glLinkProgram(quadShader);

    // Sprzątanie po kompilacji
    glDeleteShader(vQuadShader);
    glDeleteShader(fQuadShader);

    glm::vec3 lightPosTop = glm::vec3(0.0f, 3.5f, 0.0f);
    glm::vec3 lightPosLeft = glm::vec3(0.0f, 1.0f, 3.5f);
    glm::vec3 averagedLightPos = (lightPosTop + lightPosLeft) / 2.0f;
    shadowShader.SetLightPos(averagedLightPos);

    while (!glfwWindowShouldClose(win)) {
        const float currentTime = (float)glfwGetTime();
        const float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        camera->update();
        
        if (MouseController::g_leftDown) {
            camera->updateFromController();
        }

        // --- KROK 1: RENDEROWANIE SCENY (NORMALNE) ---
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glDisable(GL_STENCIL_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        shader.Use();
        shader.SetMaterial(glm::vec3(0.4f, 0.55f, 0.7f), 0.1f, 16);
        shader.SetModelMatrix(glm::mat4(1.0f));
        room.Draw();

        glm::mat4 baseTransform = glm::mat4(1.0f);
        if (pumaRobot->isAnimating) {
            // (Twój kod animacji bez zmian...)
            animationAngle += animationSpeed * deltaTime * 50.0f;
            glm::vec4 localPoint(circleRadius * cos(glm::radians(animationAngle)), 0.0f, circleRadius * sin(glm::radians(animationAngle)), 1.0f);
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(tiltAngle), tiltAxis);
            glm::vec4 rotatedPoint = rotationMatrix * localPoint;
            glm::vec3 rotatedNormal = glm::vec3(rotationMatrix * glm::vec4(targetNormal, 0.0f));
            glm::vec3 targetPos = circleCenter + glm::vec3(rotatedPoint);
            pumaRobot->ApplyInverseKinematics(targetPos, rotatedNormal);
        }
        shader.SetMaterial(glm::vec3(0.6f, 0.6f, 0.6f), 0.4f, 32);
        pumaRobot->Draw(shader, baseTransform);

        // --- KROK 2: GENEROWANIE OBJĘTOŚCI CIENIA W STENCIL BUFFER (Z-FAIL) ---
        glEnable(GL_DEPTH_CLAMP); // KLUCZOWE! Pozwala na rysowanie punktów w nieskończoności.
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // Nie rysujemy kolorów
        glDepthMask(GL_FALSE); // Nie nadpisujemy bufora głębi

        glDepthFunc(GL_LEQUAL);

        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_ALWAYS, 0, 0xFF);

        glDisable(GL_CULL_FACE); // Musimy rysować zarówno przednie, jak i tylne ściany objętości

        // Z-fail: modyfikujemy stencil tylko gdy test GŁĘBI ZAWODZI (Depth Fails)
        // Dla tylnych ścian (Back faces) -> Inkrementacja
        glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
        // Dla przednich ścian (Front faces) -> Dekrementacja
        glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

        shadowShader.Use();
        pumaRobot->DrawShadow(shadowShader, baseTransform);

        glDisable(GL_DEPTH_CLAMP);

        // --- KROK 3: RYSOWANIE CIENIA NA PODSTAWIE STENCIL BUFFER ---
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glDisable(GL_DEPTH_TEST); // Rysujemy quad na całym ekranie

        // Rysuj tylko tam, gdzie stencil nie jest równy 0 (czyli wewnątrz cienia)
        glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); // Nie modyfikuj już stencila

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Użyj prostego shadera do quada
        glUseProgram(quadShader);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glDisable(GL_BLEND);


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
    glfwWindowHint(GLFW_STENCIL_BITS, 8);

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
