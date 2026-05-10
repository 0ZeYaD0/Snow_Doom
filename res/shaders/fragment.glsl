#version 330 core
out vec4 FragColor;

in vec3 vertexColor;
in vec3 normal;
in vec2 TexCoord;

// The unified texture sampler bound in C++
uniform sampler2D u_Texture; 
uniform int useTexture;

void main()
{
    // Default to pure white so multiplying by it changes nothing for untextured map blocks
    vec4 texColor = vec4(1.0, 1.0, 1.0, 1.0); 

    if (useTexture == 1) 
    {
        texColor = texture(u_Texture, TexCoord);
        
        // Retro transparent discard for sprites
        if (texColor.a < 0.1) 
            discard;
    }

    // Fake sunlight coming from the top-right
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    vec3 norm = normalize(normal);
    float diff = max(dot(norm, lightDir), 0.0);
    
    // Add a little ambient light so the shadows aren't pitch black
    float ambient = 0.3;
    
    // Combine texture (or white), vertex color, and lighting
    vec3 finalColor = texColor.rgb * vertexColor * (diff + ambient);
    
    FragColor = vec4(finalColor, texColor.a);
}