#include <engine/graphics/buffer.h>

// VBO IMPL

VBO::VBO(const void *data, u32 size)
{
    glGenBuffers(1, &ID);
    glBindBuffer(GL_ARRAY_BUFFER, ID);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

VBO::~VBO()
{
    glDeleteBuffers(1, &ID);
}

void VBO::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, ID);
}

void VBO::Unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// VAO IMPL

VAO::VAO()
{
    glGenVertexArrays(1, &ID);
}

VAO::~VAO()
{
    glDeleteVertexArrays(1, &ID);
}

void VAO::LinkAttrib(VBO &vbo, u32 layout, u32 num_components, u32 type, u32 stride, void *offset)
{
    vbo.Bind();
    glVertexAttribPointer(layout, num_components, type, GL_FALSE, stride, offset);
    glEnableVertexAttribArray(layout);
    vbo.Unbind();
}

void VAO::Bind() const
{
    glBindVertexArray(ID);
}

void VAO::Unbind() const
{
    glBindVertexArray(0);
}

// EBO IMPL

EBO::EBO(const u32 *indices, u32 size)
{
    glGenBuffers(1, &ID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
}

EBO::~EBO()
{
    glDeleteBuffers(1, &ID);
}

void EBO::Bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

void EBO::Unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
