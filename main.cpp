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
#include "puma/DepthShader.h"
#include "puma/ParticleShader.h"
#include "puma/Particle.h"
#include "puma/Cylinder.h"

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

    glm::mat4x4 P = glm::perspective(60.0f * (float)(M_PI / 180.0), float(W) / H, 0.5f, 1000.0f);

    // Animation 
    float animationAngle = 0.0f;
    float animationSpeed = 1.5f;
    float circleRadius = 0.4f;
    glm::vec3 circleCenter = glm::vec3(-1.6f, 0.3f, 0.0f);

    float tiltAngle = -60;
    glm::vec3 tiltAxis = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 targetNormal = glm::vec3(0.0f, 1.0f, 0.0f);

    // Blacha VAO
    unsigned int sheetVAO, sheetVBO;
    float sheetSize = circleRadius * 2.0f + 0.3f; 
    float halfSize = sheetSize / 2.0f;
    float sheetVerts[] = {
        -halfSize, 0.0f, -halfSize,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
        -halfSize, 0.0f,  halfSize,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
         halfSize, 0.0f,  halfSize,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,

        -halfSize, 0.0f, -halfSize,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
         halfSize, 0.0f,  halfSize,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
         halfSize, 0.0f, -halfSize,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &sheetVAO);
    glGenBuffers(1, &sheetVBO);
    glBindVertexArray(sheetVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sheetVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sheetVerts), sheetVerts, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glm::mat4 sheetModelMatrix = glm::translate(glm::mat4(1.0f), circleCenter);
    sheetModelMatrix = glm::rotate(sheetModelMatrix, glm::radians(tiltAngle), tiltAxis);
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glViewport(0, 0, W, H);

    // Iskry
    float particle_vertices[] = {
        // Pos      // Tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    unsigned int particleVAO, vbo;
    glGenVertexArrays(1, &particleVAO);
    glGenBuffers(1, &vbo);
    glBindVertexArray(particleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_vertices), particle_vertices, GL_STATIC_DRAW);

    // Jeden atrybut vec4 (Location 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    float lastTime = 0.0f;
    bool firstFrame = true;

    SceneShader shader(P, camera.get());
    Room room;
    const char* quadVert = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    void main() { gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); }
    )";
    const char* quadFrag = R"(
    #version 330 core
    out vec4 FragColor;
    void main() { FragColor = vec4(0.0, 0.0, 0.0, 0.5); } 
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

    GLuint depthMapFBO, depthMap;
    const int SHADOW_W = 4096, SHADOW_H = 4096;

    // Tworzymy depth mape
    glGenFramebuffers(1, &depthMapFBO);
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_W, SHADOW_H,
        0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float border[] = { 1,1,1,1 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glm::vec3 lightPosTop = glm::vec3(0.0f, 3.5f, 0.0f);
    glm::vec3 lightPosLeft = glm::vec3(0.0f, 0.0f, 3.5f);
    DepthShader depthShader = DepthShader();
    ParticleShader particleShader = ParticleShader();
    ParticleSystem particleSystem = ParticleSystem(*pumaRobot);

    GLuint lineVAO, lineVBO;
    glGenVertexArrays(1, &lineVAO);
    glGenBuffers(1, &lineVBO);

    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    Cylinder cylinder = Cylinder();
    glGenVertexArrays(1, &cylinder.cylVAO);
    glGenBuffers(1, &cylinder.cylVBO);
    glBindVertexArray(cylinder.cylVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cylinder.cylVBO);
    glBufferData(GL_ARRAY_BUFFER, cylinder.cylVerts.size() * sizeof(float), cylinder.cylVerts.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Macierz modelu - ustawienie pozycji walca na podłodze i położenie go na boku
    glm::mat4 cylModelMatrix = glm::mat4(1.0f);
    cylModelMatrix = glm::translate(cylModelMatrix, glm::vec3(-2.0f, -1, -2.0f));
    cylModelMatrix = glm::rotate(cylModelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    while (!glfwWindowShouldClose(win)) {
        const float currentTime = (float)glfwGetTime();
        const float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        camera->update();
        if (MouseController::g_leftDown)
            camera->updateFromController();

        // Animacja robota
        glm::mat4 baseTransform = glm::mat4(1.0f);
        if (pumaRobot->isAnimating) {
            animationAngle += animationSpeed * deltaTime * 50.0f;
            glm::vec4 localPoint(circleRadius * cos(glm::radians(animationAngle)), 0.0f,
                circleRadius * sin(glm::radians(animationAngle)), 1.0f);
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(tiltAngle), tiltAxis);
            glm::vec4 rotatedPoint = rotationMatrix * localPoint;
            glm::vec3 rotatedNormal = glm::vec3(rotationMatrix * glm::vec4(targetNormal, 0.0f));
            glm::vec3 targetPos = circleCenter + glm::vec3(rotatedPoint);
            pumaRobot->ApplyInverseKinematics(targetPos, rotatedNormal);
        }

        particleSystem.Update(deltaTime);

        // Macierz światła 
        glm::mat4 lightView = glm::lookAt(lightPosLeft + lightPosTop, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
        glm::mat4 lightProj = glm::ortho(-10.f, 10.f, -10.f, 10.f, 0.1f, 20.f);
        glm::mat4 lightSpaceMatrix = lightProj * lightView;

        // Depth map z perspektywy światła
        glDisable(GL_STENCIL_TEST);
        glViewport(0, 0, SHADOW_W, SHADOW_H);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT); // Peter panning
        depthShader.Use();
        depthShader.SetLightSpaceMatrix(lightSpaceMatrix);
        depthShader.SetModelMatrix(glm::mat4(1.0f));
        pumaRobot->DrawShadow(depthShader, baseTransform);

        glDisable(GL_CULL_FACE);
        depthShader.SetModelMatrix(sheetModelMatrix);
        glBindVertexArray(sheetVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // --- Render ---
        glViewport(0, 0, W, H);
        glStencilMask(0xFF);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        // Renderowanie sceny bez odbicia
        glDisable(GL_STENCIL_TEST);
        shader.Use();
        shader.SetReceiveShadows(true);
        shader.SetLightSpaceMatrix(lightSpaceMatrix);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        shader.SetShadowMap();
        glActiveTexture(GL_TEXTURE0);
        shader.SetMaterial(glm::vec3(0.4f, 0.55f, 0.7f), 0.1f, 16);
        shader.SetModelMatrix(glm::mat4(1.0f));
        room.Draw();
        shader.SetMaterial(glm::vec3(0.6f, 0.6f, 0.6f), 0.6f, 32);
        pumaRobot->Draw(shader, baseTransform);

        glDisable(GL_CULL_FACE);
        shader.SetMaterial(glm::vec3(0.8f, 0.3f, 0.3f), 0.5f, 32); 
        shader.SetModelMatrix(cylModelMatrix);
        glBindVertexArray(cylinder.cylVAO);
        glDrawArrays(GL_TRIANGLES, 0, cylinder.cylVerts.size() / 8);
        glEnable(GL_CULL_FACE);

        if (pumaRobot->isAnimating) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glDepthMask(GL_FALSE);
            glDisable(GL_CULL_FACE);

            particleShader.Use();
            particleShader.SetProjection(P);
            particleShader.SetView(camera->view());

            glBindVertexArray(particleVAO);
            glLineWidth(3.0f);
            for (const auto& p : particleSystem.particles) {
                if (p.Life > 0.0f) {
                    float lineVertices[] = {
                        p.PrevPosition.x, p.PrevPosition.y, p.PrevPosition.z,
                        p.Position.x,     p.Position.y,     p.Position.z
                    };

                    particleShader.SetColor(p.Color);

                    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
                    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(lineVertices), lineVertices);

                    glBindVertexArray(lineVAO);
                    glDrawArrays(GL_LINES, 0, 2);

                }
            }
            glLineWidth(1.0f);
            glBindVertexArray(0);
            glDisable(GL_BLEND);
            glDepthMask(GL_TRUE);
            glEnable(GL_CULL_FACE);
        }

		// Zapisanie maski lustra do bufora szablonu
        glEnable(GL_STENCIL_TEST);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF); 
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glDepthMask(GL_FALSE);
		glDisable(GL_CULL_FACE);
        shader.SetModelMatrix(sheetModelMatrix);
        glBindVertexArray(sheetVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glDepthMask(GL_TRUE);
		glEnable(GL_CULL_FACE);

        // Rysowanie odbitej sceny
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_EQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);

        glm::mat4 reflectionMatrix = sheetModelMatrix * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, -1.0f, 1.0f)) * glm::inverse(sheetModelMatrix);
        shader.SetMaterial(glm::vec3(0.4f, 0.55f, 0.7f), 0.1f, 16);
        shader.SetModelMatrix(reflectionMatrix);
        shader.SetReceiveShadows(false);
        glDepthMask(GL_FALSE);
        room.Draw();
        glDepthMask(GL_TRUE);
        shader.SetMaterial(glm::vec3(0.6f, 0.6f, 0.6f), 0.4f, 32);
        pumaRobot->Draw(shader, reflectionMatrix * baseTransform);
        
        if (pumaRobot->isAnimating)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glDepthMask(GL_FALSE);
            glDisable(GL_CULL_FACE);

            particleShader.Use();
            particleShader.SetProjection(P);
            particleShader.SetView(camera->view() * reflectionMatrix);

            glBindVertexArray(lineVAO);
            glLineWidth(3.0f);

            for (const auto& p : particleSystem.particles) {
                if (p.Life > 0.0f) {
                    float lineVertices[] = {
                        p.PrevPosition.x, p.PrevPosition.y, p.PrevPosition.z,
                        p.Position.x,     p.Position.y,     p.Position.z
                    };

                    particleShader.SetColor(p.Color);

                    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
                    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(lineVertices), lineVertices);

                    glDrawArrays(GL_LINES, 0, 2);
                }
            }
            glLineWidth(1.0f);
            glBindVertexArray(0);

            glDisable(GL_BLEND);
            glDepthMask(GL_TRUE);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
        }

        // Rysowanie powierzchni blachy
        glStencilFunc(GL_EQUAL, 1, 0xFF);
        glEnable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

        shader.SetMaterial(glm::vec3(0.65f, 0.65f, 0.7f), 0.8f, 64, 0.5f);
        shader.SetModelMatrix(sheetModelMatrix);
        glBindVertexArray(sheetVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &sheetVAO);
    glDeleteBuffers(1, &sheetVBO);
    glfwTerminate();
    return 0;
}

GLFWwindow* initWindow(int& H, int& W) {
    if (!glfwInit()) return nullptr;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
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
    P = glm::perspective(60.0f * (float)(M_PI / 180.0), float(W) / H, 0.5f, 1000.0f);
    return true;
}
