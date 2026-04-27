#include <engine/graphics/mesh.h>

#include <cstddef>

Mesh::Mesh(const vector<Vertex> &vertices, const vector<u32> &indices)
    : vertices(vertices), indices(indices)
{
    vao = new VAO();
    vao->Bind();

    vbo = new VBO(vertices.data(), vertices.size() * sizeof(Vertex));
    ebo = new EBO(indices.data(), indices.size() * sizeof(u32));
    
    // location 0: position
    vao->LinkAttrib(*vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, position));
    // location 1: normal
    vao->LinkAttrib(*vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // location 2: tex_uv
    vao->LinkAttrib(*vbo, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, tex_uv));
    // location 3: color
    vao->LinkAttrib(*vbo, 3, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, color));

    vao->Unbind();
    vbo->Unbind();
    ebo->Unbind();
}

void Mesh::Draw(const Shader &shader) const
{
    vao->Bind();
        glDrawElements(GL_TRIANGLES, static_cast<i32>(indices.size()), GL_UNSIGNED_INT, 0);
    vao->Unbind();
}
