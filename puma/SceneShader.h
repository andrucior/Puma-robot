#pragma once
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "view/Camera.h"

class SceneShader {
	GLuint roomVAO, roomVBO, shader;
	Camera* camera;
	glm::mat4& P;
	glm::vec3 lightPos;

	const char* vertexSrc = R"(
		#version 330 core

		layout(location = 0) in vec3 aPos;
		layout(location = 1) in vec3 aNormal;

		uniform mat4 MVP;
		uniform mat4 model;

		out vec3 FragPos;
		out vec3 Normal;

		void main()
		{
			FragPos = vec3(model * vec4(aPos, 1.0));
			Normal = mat3(transpose(inverse(model))) * aNormal;

			gl_Position = MVP * vec4(aPos, 1.0);
		}
	)";


	const char* fragmentSrc = R"(
		#version 330 core

		out vec4 FragColor;

		in vec3 FragPos;
		in vec3 Normal;

		uniform vec3 lightPos;
		uniform vec3 viewPos;

		void main()
		{
			// ambient
			float ambientStrength = 0.2;
			vec3 ambient = ambientStrength * vec3(1.0);

			// diffuse
			vec3 norm = normalize(Normal);
			vec3 lightDir = normalize(lightPos - FragPos);

			float diff = max(dot(norm, lightDir), 0.0);
			vec3 diffuse = diff * vec3(1.0);

			// specular
			float specStrength = 0.1;
			vec3 viewDir = normalize(viewPos - FragPos);
			vec3 reflectDir = reflect(-lightDir, norm);

			float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
			vec3 specular = specStrength * spec * vec3(1.0);

			vec3 color = (ambient + diffuse + specular) * vec3(0.4, 0.55, 0.7);

			FragColor = vec4(color, 1.0);
		}
	)";

	float room[216] = {
		// vertex | normal
		
		// floor
		-5,0,-5,   0,1,0,
		 5,0,-5,   0,1,0,
		 5,0, 5,   0,1,0,

		-5,0,-5,   0,1,0,
		 5,0, 5,   0,1,0,
		-5,0, 5,   0,1,0,

		// ceiling
		-5,5,-5,   0,-1,0,
		 5,5,-5,   0,-1,0,
		 5,5, 5,   0,-1,0,
		
		-5,5,-5,   0,-1,0,
		 5,5, 5,   0,-1,0,
		-5,5, 5,   0,-1,0,

		// back wall
		-5,0,-5,   0,0,1,
		5,0,-5,    0,0,1,
		5,5,-5,    0,0,1,

		-5,0,-5,   0,0,1,
		5,5,-5,    0,0,1,
		-5,5,-5,   0,0,1,

		// front wall
		-5,0,5,    0,0,-1,
		 5,0,5,    0,0,-1,
		 5,5,5,    0,0,-1,

		-5,0,5,    0,0,-1,
		 5,5,5,    0,0,-1,
		-5,5,5,    0,0,-1,

		// left wall
		-5,0,-5,   1,0,0,
		-5,0,5,    1,0,0,
		-5,5,5,    1,0,0,
		
		-5,0,-5,   1,0,0,
		-5,5,5,    1,0,0,
		-5,5,-5,   1,0,0,

		// right wall
		5,0,-5,   -1,0,0,
		5,0,5,    -1,0,0,
		5,5,5,    -1,0,0,

		5,0,-5,   -1,0,0,
		5,5,5,    -1,0,0,
		5,5,-5,    -1,0,0,
	};

	GLuint compileShader(GLenum type, const char* src);

	GLuint createProgram();

public:
	SceneShader(glm::mat4& P, Camera* camera);

	void Draw();
};

