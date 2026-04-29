#pragma once
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "view/Camera.h"

class SceneShader {
	GLuint shader;
	Camera* camera;
	glm::mat4& P;
	glm::vec3 lightPosTop;
	glm::vec3 lightPosLeft;

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

		uniform vec3 lightPosTop;
		uniform vec3 lightPosLeft;
		uniform vec3 viewPos;
		uniform vec3 objectColor;
		uniform float specStrength;
		uniform int shininess;

		void main()
		{
			// ambient
			float ambientStrength = 0.2;
			vec3 ambient = ambientStrength * vec3(1.0);

			vec3 norm = normalize(Normal);
			vec3 viewDir = normalize(viewPos - FragPos);

			// light top
			vec3 lightDirTop = normalize(lightPosTop - FragPos);
			float diffTop = max(dot(norm, lightDirTop), 0.0);
			vec3 diffuseTop = diffTop * vec3(1.0);
			vec3 reflectDirTop = reflect(-lightDirTop, norm);
			float specTop = pow(max(dot(viewDir, reflectDirTop), 0.0), float(shininess));
			vec3 specularTop = specStrength * specTop * vec3(1.0);

			// light left
			vec3 lightDirLeft = normalize(lightPosLeft - FragPos);
			float diffLeft = max(dot(norm, lightDirLeft), 0.0);
			vec3 diffuseLeft = diffLeft * vec3(1.0);
			vec3 reflectDirLeft = reflect(-lightDirLeft, norm);
			float specLeft = pow(max(dot(viewDir, reflectDirLeft), 0.0), float(shininess));
			vec3 specularLeft = specStrength * specLeft * vec3(1.0);

			vec3 diffuse = diffuseTop + diffuseLeft;
			vec3 specular = specularTop + specularLeft;

			vec3 color = (ambient + diffuse + specular) * objectColor;

			FragColor = vec4(color, 1.0);
		}
	)";

	GLuint compileShader(GLenum type, const char* src);

	GLuint createProgram();

public:
	SceneShader(glm::mat4& P, Camera* camera);

	void Use();
	void SetModelMatrix(const glm::mat4& model);
	void SetMaterial(const glm::vec3& color, float specStrength, int shininess);
};

