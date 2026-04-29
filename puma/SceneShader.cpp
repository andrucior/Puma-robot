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
	shader = createProgram();
	lightPosTop = glm::vec3(0.0f, 3.5f, 0.0f);
	lightPosLeft = glm::vec3(0.0f, 1.0f, 3.5f);
}

void SceneShader::Use()
{
	glUseProgram(shader);
	glUniform3fv(glGetUniformLocation(shader, "lightPosTop"), 1, glm::value_ptr(lightPosTop));
	glUniform3fv(glGetUniformLocation(shader, "lightPosLeft"), 1, glm::value_ptr(lightPosLeft));
	glUniform3fv(glGetUniformLocation(shader, "viewPos"), 1, glm::value_ptr(camera->cameraPosition));
}

void SceneShader::SetModelMatrix(const glm::mat4& model)
{
	glUseProgram(shader);
	glm::mat4 view = camera->view();
	glm::mat4 mvp = P * view * model;

	GLuint loc = glGetUniformLocation(shader, "MVP");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mvp));

	GLuint modelLoc = glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
}

void SceneShader::SetMaterial(const glm::vec3& color, float specStrength, int shininess)
{
	glUseProgram(shader);
	glUniform3fv(glGetUniformLocation(shader, "objectColor"), 1, glm::value_ptr(color));
	glUniform1f(glGetUniformLocation(shader, "specStrength"), specStrength);
	glUniform1i(glGetUniformLocation(shader, "shininess"), shininess);
}


