#pragma once
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "view/Camera.h"

class MirrorShader {
	GLuint shader;
	Camera* camera;
	glm::mat4& P;

	const char* vertexSrc = R"(
		#version 330 core

		layout (location = 0) in vec3 aPos;
		layout (location = 1) in vec3 aNormal;
		layout (location = 2) in vec2 aTexCoords;

		out vec2 TexCoords;
		out vec3 Normal;
		out vec3 FragPos;

		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;

		void main() 
		{
			FragPos = vec3(model * vec4(aPos, 1.0));
    
			Normal = mat3(transpose(inverse(model))) * aNormal;  
    
			TexCoords = aTexCoords;

			gl_Position = projection * view * vec4(FragPos, 1.0);
		}
	)";


	const char* fragmentSrc = R"(
		#version 330 core

		out vec4 FragColor;

		in vec2 TexCoords;
		in vec3 Normal;
		in vec3 FragPos;

		uniform sampler2D mirrorTexture;
		uniform vec4 u_glassTint; 

		void main() 
		{
			vec4 finalColor;
			vec4 texColor = texture(mirrorTexture, TexCoords);
        
			finalColor = texColor * u_glassTint;

			FragColor = finalColor;
		}
	)";

	GLuint compileShader(GLenum type, const char* src);

	GLuint createProgram();

public:
	MirrorShader(glm::mat4& P, Camera* camera);

	void Use();
	void SetModelMatrix(const glm::mat4& model);
};

