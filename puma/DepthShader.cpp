#include "DepthShader.h"

GLuint DepthShader::compileShader(GLenum type, const char* src)
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

GLuint DepthShader::createProgram()
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

DepthShader::DepthShader()
{
	shader = createProgram();
}

void DepthShader::Use() const
{
	glUseProgram(shader);
}

void DepthShader::SetModelMatrix(const glm::mat4& model) const
{
	glUseProgram(shader);
	GLuint modelLoc = glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
}

void DepthShader::SetLightSpaceMatrix(const glm::mat4& lightSpaceMtx) const
{
	glUseProgram(shader);
	GLuint lightLoc = glGetUniformLocation(shader, "lightSpaceMatrix");
	glUniformMatrix4fv(lightLoc, 1, GL_FALSE, glm::value_ptr(lightSpaceMtx));
}
