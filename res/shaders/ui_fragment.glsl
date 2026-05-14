#version 330 core
in vec2 TexCoords;
in vec4 TintColor;
in float IsTextured;

out vec4 FragColor;

uniform sampler2D image;

void main() 
{
    if (IsTextured > 0.5) 
    {
        vec4 texColor = texture(image, TexCoords);
        
        if(texColor.a < 0.1) {
            discard; 
        }
        
        FragColor = texColor * TintColor;
    } 
    else 
    {
        FragColor = TintColor;
    }
}