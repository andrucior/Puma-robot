#include "SceneShader.h"

GLuint SceneShader::compileShader(GLenum type, const char* src)
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

GLuint SceneShader::createProgram()
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

SceneShader::SceneShader(glm::mat4& P, Camera* camera)
	: P(P), camera(camera)
{
	glGenVertexArrays(1, &roomVAO);
	glGenBuffers(1, &roomVBO);

	glBindVertexArray(roomVAO);
	glBindBuffer(GL_ARRAY_BUFFER, roomVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(room), room, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	shader = createProgram();
	lightPos = glm::vec3(0.0f, 3.5f, 0.0f);
}

void SceneShader::Draw()
{
	glDisable(GL_CULL_FACE);

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = camera->view();
	glm::mat4 mvp = P * view * model;

	glUseProgram(shader);

	GLuint loc = glGetUniformLocation(shader, "MVP");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mvp));
	glUniform3fv(glGetUniformLocation(shader, "lightPos"), 1, glm::value_ptr(lightPos));
	glUniform3fv(glGetUniformLocation(shader, "viewPos"), 1, glm::value_ptr(camera->cameraPosition));

	GLuint modelLoc = glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(roomVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glEnable(GL_CULL_FACE);
}

