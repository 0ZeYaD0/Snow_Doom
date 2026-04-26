#pragma once

#include <defines.h>

#include <graphics/buffer.h>
#include <graphics/shader.h>

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
    vector<u32>    indices;

    Mesh(const vector<Vertex> &vertices, const vector<u32> &indices);
    void Draw(const Shader &shader) const;

private:
    VAO *vao;
    VBO *vbo;
    EBO *ebo;
};