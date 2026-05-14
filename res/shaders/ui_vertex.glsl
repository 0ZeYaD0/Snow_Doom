#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec4 aColor;
layout (location = 3) in float aIsTextured;

out vec2 TexCoords;
out vec4 TintColor;
out float IsTextured;

uniform mat4 projection;

void main() 
{
    TexCoords = aTexCoords;
    TintColor = aColor;
    IsTextured = aIsTextured;
    
    // The vertex position is already translated, scaled, and rotated by the CPU now!
    gl_Position = projection * vec4(aPos, 0.0, 1.0);
}