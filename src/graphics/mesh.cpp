#include <graphics/mesh.h>

#include <cstddef>

Mesh::Mesh(const vector<Vertex> &vertices, const vector<u32> &indices)
    : vertices(vertices), indices(indices)
{
    vao = new VAO();
    vao->Bind();

    vbo = new VBO(vertices.data(), vertices.size() * sizeof(Vertex));
    ebo = new EBO(indices.data(), indices.size() * sizeof(u32));

    vao->LinkAttrib(*vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void *) 0);
    vao->LinkAttrib(*vbo, 1, 2, GL_FLOAT, sizeof(Vertex), (void *) offsetof(Vertex, tex_uv));
    vao->LinkAttrib(*vbo, 2, 3, GL_FLOAT, sizeof(Vertex), (void *) offsetof(Vertex, normal));

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
