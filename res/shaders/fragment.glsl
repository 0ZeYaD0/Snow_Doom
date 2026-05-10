#version 330 core
out vec4 FragColor;

in vec3 vertexColor;
in vec3 normal;
in vec2 TexCoord;

uniform sampler2D spriteTexture;
uniform int useTexture;

void main()
{
    vec4 texColor = vec4(1.0, 1.0, 1.0, 1.0); 

    if (useTexture == 1) 
    {
        texColor = texture(spriteTexture, TexCoord);
        if (texColor.a < 0.1) 
            discard;
    }

    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0)); 
    vec3 norm = normalize(normal);
    float diff = max(dot(norm, lightDir), 0.0);
    float ambient = 0.3;

    vec3 finalColor = texColor.rgb * vertexColor * (diff + ambient);

    FragColor = vec4(finalColor, texColor.a);
}