#pragma once

#include <engine/core/defines.h>

#include <engine/graphics/buffer.h>
#include <engine/graphics/shader.h>

#include <vector>
using std::vector;

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_uv;
    glm::vec3 color;
};

class Mesh
{
public:
    vector<Vertex> vertices;
    vector<u32> indices;
    Mesh() : vao(nullptr), vbo(nullptr), ebo(nullptr) {}

    Mesh(const std::vector<Vertex> &vertices, const std::vector<u32> &indices);
    void Draw(const Shader &shader) const;

private:
    VAO *vao;
    VBO *vbo;
    EBO *ebo;
};