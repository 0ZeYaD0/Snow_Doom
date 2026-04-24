#pragma once

#include <defines.h>

#include <glad/glad.h>
#include <cstdint>

class VBO 
{
public:
    u32 ID;
    
    VBO(const void *data, u32 size);
    ~VBO();

    void Bind() const;
    void Unbind() const;
};

class VAO
{
public:
    u32 ID;

    VAO();
    ~VAO();

    void LinkAttrib(VBO &vbo, u32 layout, u32 num_components, u32 type, u32 stride, void *offset);

    void Bind() const;
    void Unbind() const;
};

class EBO
{
public:
    u32 ID;
    
    EBO(const u32 *indices, u32 size);
    ~EBO();

    void Bind() const;
    void Unbind() const;
};