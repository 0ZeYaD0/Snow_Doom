#include <graphics/shader.h>

#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <sstream>

Shader::Shader(const string &vertex_path, const string &fragment_path)
{
    string vert_code = LoadShaderSource(vertex_path);
    string frag_code = LoadShaderSource(fragment_path);

    u32 vertex = CompileShader(GL_VERTEX_SHADER, vert_code);
    u32 fragment = CompileShader(GL_FRAGMENT_SHADER, frag_code);

    ID = glCreateProgram();

    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    i32 success;
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if(!success) {
        char info_log[1024];
        glGetProgramInfoLog(ID, 1024, nullptr, info_log);
        throw std::runtime_error(string("shader linking failed: ") + info_log);
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::~Shader()
{
    glDeleteProgram(ID);
}

void Shader::Use() const
{
    glUseProgram(ID);
}

void Shader::SetMat4(const string &name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::SetVec4(const string &name, f32 x, f32 y, f32 z, f32 w) const
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

string Shader::LoadShaderSource(const string &path)
{
    std::ifstream file(path);
    if(!file) throw std::runtime_error("failed to open shader file" + path);

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

u32 Shader::CompileShader(GLenum type, const string &source)
{
    u32 shader = glCreateShader(type);

    const char *src = source.c_str();

    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    i32 success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        char info_log[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, info_log);
        throw std::runtime_error(string("shader compilation failed: ") + info_log);
    }

    return shader;
}
