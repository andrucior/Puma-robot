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
		layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

		out vec2 TexCoords;
		out vec4 ParticleColor;

		uniform mat4 projection;
		uniform mat4 view;
		uniform vec3 offset;
		uniform vec4 color;

		void main() {
            float scale = 0.05f; // Realistyczna skala dla iskier
            TexCoords = vertex.zw;
            ParticleColor = color;
            
            // Obliczamy pozycję w świecie: pozycja środka (offset) + przesunięcie wierzchołka (vertex.xy)
            // Dzięki temu cząsteczka zawsze jest zwrócona "przodem" (quad)
			gl_Position = projection * view * vec4((vec3(vertex.xy, 0.0) * scale) + offset, 1.0);        }
	)";

	const char* fragmentSrc = R"(
		#version 330 core
		in vec2 TexCoords;
		in vec4 ParticleColor;
		out vec4 FragColor;

		void main()
		{
			// Obliczamy wektor od środka (0.5, 0.5) do aktualnego punktu
			vec2 center = vec2(0.5, 0.5);
			float dist = distance(TexCoords, center);

			// Tworzymy miękki spadek jasności (radial gradient)
			// Im bliżej środka (dist -> 0), tym jasność bliższa 1.0
			float glow = exp(-5.0 * dist); // Funkcja wykładnicza daje ładny efekt blasku
        
			// Opcjonalnie: odcinamy krawędzie, żeby nie było widać kwadratu
			float alpha = smoothstep(0.5, 0.2, dist);

			FragColor = ParticleColor * glow * alpha;
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