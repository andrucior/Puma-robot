#pragma once
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

class DepthShader {
	GLuint shader;

	const char* vertexSrc = R"(
		#version 330 core
		layout(location = 0) in vec3 aPos;

		uniform mat4 lightSpaceMatrix;
		uniform mat4 model;
		void main() {
			gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
		}
	)";

	const char* fragmentSrc = R"(
		#version 330 core
		void main() {}
	)";

	GLuint compileShader(GLenum type, const char* src);

	GLuint createProgram();

public:
	DepthShader();

	void Use() const;

	void SetModelMatrix(const glm::mat4& model) const;

	void SetLightSpaceMatrix(const glm::mat4& lightSpaceMtx) const;
};

