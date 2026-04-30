#include "MirrorShader.h"

GLuint MirrorShader::compileShader(GLenum type, const char* src)
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

GLuint MirrorShader::createProgram()
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

MirrorShader::MirrorShader(glm::mat4& P, Camera* camera)
	: P(P), camera(camera)
{
	shader = createProgram();
}

void MirrorShader::Use()
{
	glUseProgram(shader);
	glUniform4fv(glGetUniformLocation(shader, "u_glassTint"), 1, glm::value_ptr(glm::vec4(0.65f, 0.65f, 0.7f, 0.6f)));
	glUniform1i(glGetUniformLocation(shader, "mirrorTexture"), 0);
}

void MirrorShader::SetModelMatrix(const glm::mat4& model)
{
	glUseProgram(shader);
	glm::mat4 view = camera->view();

	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(P));
}


