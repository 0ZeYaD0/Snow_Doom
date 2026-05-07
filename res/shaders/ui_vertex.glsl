#version 330 core
layout (location = 0) in vec2 aPos;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 projection;

void main()
{
    // Pass the raw 0.0 to 1.0 position directly to the fragment shader as the UV coordinate
    TexCoords = aPos;
    
    gl_Position = projection * model * vec4(aPos, 0.0, 1.0);
}