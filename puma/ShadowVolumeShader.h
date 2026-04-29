#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "view/Camera.h"

class ShadowVolumeShader {
    GLuint shader;
    Camera* camera;
    glm::mat4& P;

    const char* vertexSrc = R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        uniform mat4 model;
        
        void main() {
            gl_Position = model * vec4(aPos, 1.0);
        }
    )";

    const char* geometrySrc = R"(
        #version 330 core
        layout(triangles_adjacency) in;
        layout(triangle_strip, max_vertices = 18) out;

        uniform vec3 lightPos;
        uniform mat4 VP; // View * Projection

        // Funkcja do rzutowania wektora od œwiat³a w nieskoñczonoœæ
        vec4 Extrude(vec3 pos) {
            vec3 dir = pos - lightPos;
            return VP * vec4(dir, 0.0);
        }

        void main() {
            vec3 v0 = gl_in[0].gl_Position.xyz;
            vec3 v1 = gl_in[1].gl_Position.xyz;
            vec3 v2 = gl_in[2].gl_Position.xyz;
            vec3 v3 = gl_in[3].gl_Position.xyz;
            vec3 v4 = gl_in[4].gl_Position.xyz;
            vec3 v5 = gl_in[5].gl_Position.xyz;

            vec3 normal = cross(v2 - v0, v4 - v0);
            if (length(normal) < 0.0001) return; // Zabezpieczenie przed degeneracj¹
            
            vec3 normNormal = normalize(normal);
            vec3 lightDir = normalize(lightPos - v0);

            if (dot(normNormal, lightDir) < 0.0) return; 

            // 1. Zaœlepka przednia
            gl_Position = VP * vec4(v0, 1.0); EmitVertex();
            gl_Position = VP * vec4(v2, 1.0); EmitVertex();
            gl_Position = VP * vec4(v4, 1.0); EmitVertex();
            EndPrimitive();

            // 2. Zaœlepka tylna
            gl_Position = Extrude(v4); EmitVertex();
            gl_Position = Extrude(v2); EmitVertex();
            gl_Position = Extrude(v0); EmitVertex();
            EndPrimitive();

            // 3. SprawdŸ krawêdzie pod k¹tem bycia Zarysem
            
            // KrawêdŸ v0 -> v2 (S¹siad V1)
            vec3 normalA = cross(v1 - v0, v2 - v0);
            bool extrudeA = false;
            if (length(normalA) < 0.0001) extrudeA = true;
            else if (dot(normalize(normalA), normalize(lightPos - v0)) <= 0.0) extrudeA = true;

            if (extrudeA) {
                gl_Position = VP * vec4(v0, 1.0); EmitVertex();
                gl_Position = Extrude(v0);        EmitVertex();
                gl_Position = VP * vec4(v2, 1.0); EmitVertex();
                gl_Position = Extrude(v2);        EmitVertex();
                EndPrimitive();
            }

            // KrawêdŸ v2 -> v4 (S¹siad V3)
            vec3 normalB = cross(v3 - v2, v4 - v2);
            bool extrudeB = false;
            if (length(normalB) < 0.0001) extrudeB = true;
            else if (dot(normalize(normalB), normalize(lightPos - v2)) <= 0.0) extrudeB = true;

            if (extrudeB) {
                gl_Position = VP * vec4(v2, 1.0); EmitVertex();
                gl_Position = Extrude(v2);        EmitVertex();
                gl_Position = VP * vec4(v4, 1.0); EmitVertex();
                gl_Position = Extrude(v4);        EmitVertex();
                EndPrimitive();
            }

            // KrawêdŸ v4 -> v0 (S¹siad V5)
            vec3 normalC = cross(v5 - v4, v0 - v4);
            bool extrudeC = false;
            if (length(normalC) < 0.0001) extrudeC = true;
            else if (dot(normalize(normalC), normalize(lightPos - v4)) <= 0.0) extrudeC = true;

            if (extrudeC) {
                gl_Position = VP * vec4(v4, 1.0); EmitVertex();
                gl_Position = Extrude(v4);        EmitVertex();
                gl_Position = VP * vec4(v0, 1.0); EmitVertex();
                gl_Position = Extrude(v0);        EmitVertex();
                EndPrimitive();
            }
        }
    )";

    const char* fragmentSrc = R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(1.0, 1.0, 0.0, 1.0);
        }
    )";

    GLuint compileShader(GLenum type, const char* src);
    GLuint createProgram();

public:
    ShadowVolumeShader(glm::mat4& P, Camera* camera);
    void Use();
    void SetModelMatrix(const glm::mat4& model);
    void SetLightPos(const glm::vec3& pos);
};