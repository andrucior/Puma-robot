#include "RobotShader.h"

GLuint RobotShader::compileShader(GLenum type, const char* src)
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
        std::cout << "RobotShader compilation error:\n" << infoLog << std::endl;
    }

    return shader;
}

GLuint RobotShader::createProgram()
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
        std::cout << "RobotShader program linking error:\n" << infoLog << std::endl;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

RobotShader::RobotShader(glm::mat4& P, Camera* camera)
    : P(P), camera(camera)
{
    programId = createProgram();
    lightPos = glm::vec3(0.0f, 3.5f, 0.0f);
}

void RobotShader::Use()
{
    glUseProgram(programId);

    glUniform3fv(glGetUniformLocation(programId, "lightPos"), 1, glm::value_ptr(lightPos));
    glUniform3fv(glGetUniformLocation(programId, "viewPos"), 1, glm::value_ptr(camera->cameraPosition));
}

void RobotShader::SetModelMatrix(const glm::mat4& model)
{
    glm::mat4 view = camera->view();
    glm::mat4 mvp = P * view * model;

    GLuint loc = glGetUniformLocation(programId, "MVP");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mvp));

    GLuint modelLoc = glGetUniformLocation(programId, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
}
