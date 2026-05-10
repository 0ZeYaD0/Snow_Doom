#pragma once

#include <engine/graphics/mesh.h>

inline Mesh BuildSpriteQuad()
{
    std::vector<Vertex> verts = {
        // Position               // Normal              // UVs          // Color (White so it doesn't tint the texture)
        {{-0.5f, 0.0f, 0.0f},     {0.0f, 0.0f, 1.0f},    {0.0f, 0.0f},   {1.0f, 1.0f, 1.0f}}, // Bottom Left
        {{ 0.5f, 0.0f, 0.0f},     {0.0f, 0.0f, 1.0f},    {1.0f, 0.0f},   {1.0f, 1.0f, 1.0f}}, // Bottom Right
        {{ 0.5f, 2.0f, 0.0f},     {0.0f, 0.0f, 1.0f},    {1.0f, 1.0f},   {1.0f, 1.0f, 1.0f}}, // Top Right
        {{-0.5f, 2.0f, 0.0f},     {0.0f, 0.0f, 1.0f},    {0.0f, 1.0f},   {1.0f, 1.0f, 1.0f}}  // Top Left
    };

    std::vector<u32> indices = {
        0, 1, 2,  // First triangle
        2, 3, 0   // Second triangle
    };

    return Mesh(verts, indices);
}