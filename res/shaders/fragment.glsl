#version 330 core
out vec4 FragColor;

in vec2 TexCoords; // Make sure your vertex shader passes this out!
in vec3 vertexColor;
in vec3 normal;
in vec2 TexCoord;

uniform sampler2D spriteTexture;
uniform int useTexture;

// The texture sampler we will bind in C++
uniform sampler2D u_Texture;

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
    // Fake sunlight coming from the top-right
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    vec3 norm = normalize(normal);
    float diff = max(dot(norm, lightDir), 0.0);
    
    // Add a little ambient light so the shadows aren't pitch black
    float ambient = 0.3;
    
    // Sample the color from the texture at the current UV coordinate
    vec4 texColor = texture(u_Texture, TexCoords);

    // Multiply the texture color by the lighting calculations
    vec3 finalColor = texColor.rgb * (diff + ambient);
    
    FragColor = vec4(finalColor, texColor.a);
}