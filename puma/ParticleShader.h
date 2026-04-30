#pragma once
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

class ParticleShader
{
	GLuint shader;

	const char* vertexSrc = R"(
		#version 330 core
		layout (location = 0) in vec3 aPos;
		uniform mat4 projection;
		uniform mat4 view;
		uniform vec4 color;
		out vec4 ParticleColor;
		void main() {
			ParticleColor = color;
			gl_Position = projection * view * vec4(aPos, 1.0);
		}
	)";

	const char* fragmentSrc = R"(
		#version 330 core
		in vec4 ParticleColor;
		out vec4 FragColor;
		void main() {
			FragColor = ParticleColor;
		}
	)";

	GLuint compileShader(GLenum type, const char* src)
	{
		GLuint shader = glCreateShader(type);
		glShaderSource(shader, 1, &src, nullptr);
		glCompileShader(shader);

		int success;
		char infoLog[1024];

		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, nullptr, infoLog);

			std::cout << "Shader compilation error:\n"
				<< infoLog << std::endl;
		}

		return shader;
	}

	GLuint createProgram()
	{
		GLuint vs = compileShader(GL_VERTEX_SHADER, vertexSrc);
		GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentSrc);

		GLuint program = glCreateProgram();
		glAttachShader(program, vs);
		glAttachShader(program, fs);

		glLinkProgram(program);
		int success;
		char infoLog[1024];

		glGetProgramiv(program, GL_LINK_STATUS, &success);

		if (!success)
		{
			glGetProgramInfoLog(program, 1024, nullptr, infoLog);

			std::cout << "Program linking error:\n"
				<< infoLog << std::endl;
		}

		glDeleteShader(vs);
		glDeleteShader(fs);

		return program;
	}
public:
	ParticleShader() {
		shader = createProgram();
	}

	void Use() { glUseProgram(shader); }

	void SetOffset(glm::vec3 offset)
	{
		Use();
		glUniform3fv(glGetUniformLocation(shader, "offset"), 1, glm::value_ptr(offset));
	}

	void SetColor(glm::vec4 color) 
	{
		Use();
		glUniform4fv(glGetUniformLocation(shader, "color"), 1, glm::value_ptr(color));
	}

	void SetProjection(const glm::mat4& projection) 
	{
		Use();
		glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	}

	void SetView(const glm::mat4& view)
	{
		Use();
		glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view));
	}
};