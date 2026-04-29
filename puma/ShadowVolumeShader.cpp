#include "ShadowVolumeShader.h"

GLuint ShadowVolumeShader::compileShader(GLenum type, const char* src)
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
        std::cout << "Shadow Volume Shader compilation error:\n" << infoLog << std::endl;
    }
    return shader;
}

GLuint ShadowVolumeShader::createProgram()
{
    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexSrc);
    GLuint gs = compileShader(GL_GEOMETRY_SHADER, geometrySrc);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentSrc);

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, gs);
    glAttachShader(program, fs);

    glLinkProgram(program);
    
    int success;
    char infoLog[1024];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 1024, nullptr, infoLog);
        std::cout << "Shadow Volume Program linking error:\n" << infoLog << std::endl;
    }

    glDeleteShader(vs);
    glDeleteShader(gs);
    glDeleteShader(fs);

    return program;
}

ShadowVolumeShader::ShadowVolumeShader(glm::mat4& P, Camera* camera)
    : P(P), camera(camera)
{
    shader = createProgram();
}

void ShadowVolumeShader::Use()
{
    glUseProgram(shader);
    glm::mat4 view = camera->view();
    glm::mat4 vp = P * view;
    GLuint vpLoc = glGetUniformLocation(shader, "VP");
    glUniformMatrix4fv(vpLoc, 1, GL_FALSE, glm::value_ptr(vp));
}

void ShadowVolumeShader::SetLightPos(const glm::vec3& pos)
{
    glUseProgram(shader);
    glUniform3fv(glGetUniformLocation(shader, "lightPos"), 1, glm::value_ptr(pos));
}

void ShadowVolumeShader::SetModelMatrix(const glm::mat4& model)
{
    GLuint modelLoc = glGetUniformLocation(shader, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
}