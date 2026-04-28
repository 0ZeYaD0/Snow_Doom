#version 330 core
layout (location = 0) in vec2 aPos;

uniform mat4 projection;
uniform mat4 model;

out vec2 localPos; // Send the local coordinates to the fragment shader

void main()
{
    localPos = aPos; // aPos is exactly 0.0 to 1.0 within our square!
    gl_Position = projection * model * vec4(aPos, 0.0, 1.0);
}