#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D image;
uniform vec4 color;
uniform bool use_texture; // Flag to switch modes

void main()
{
    if (use_texture)
    {
        vec4 texColor = texture(image, TexCoords);
        if(texColor.a < 0.1) discard; // Discard transparent pixels
        FragColor = texColor * color; // Multiply by color for tinting
    }
    else
    {
        FragColor = color; // Solid color for standard shapes
    }
}