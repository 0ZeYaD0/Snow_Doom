#pragma once

#include <defines.h>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
using std::string;

class Shader
{
public:
    u32 ID;

    Shader(const string& vertex_path, const string& fragment_path);
    ~Shader();

    void Use() const;
    void SetMat4(const string &name, const glm::mat4& mat) const;
    void SetVec4(const string &name, f32 x, f32 y, f32 z, f32 w) const;

private:
    string LoadShaderSource(const string &path);
    u32 CompileShader(GLenum type, const string &source);

};