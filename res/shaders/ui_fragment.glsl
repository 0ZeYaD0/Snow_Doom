#version 330 core
out vec4 FragColor;

in vec2 localPos; // Receive from vertex shader

uniform vec4 color;       // The "full" color (cyan)
uniform vec4 bg_color;    // The "empty" color (dark gray)
uniform float fill_amount; // 0.0 (empty) to 1.0 (full)

void main()
{
    // localPos.y goes from 0.0 (top) to 1.0 (bottom).
    // We flip it so the fill rises from the bottom to the top.
    if ((1.0 - localPos.y) <= fill_amount) 
    {
        FragColor = color;
    } 
    else 
    {
        FragColor = bg_color;
    }
}