#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

// Texture variables
uniform sampler2D image;
uniform int use_texture; 
uniform vec2 uv_scale;
uniform vec2 uv_offset;

// UI variables
uniform vec4 color;
uniform vec4 bg_color;
uniform float fill_amount;

void main()
{
    if (use_texture == 1)
    {
        // 1. Flip the Y coordinate specifically for images so they aren't upside down
        vec2 flipped_uv = vec2(TexCoords.x, 1.0 - TexCoords.y);
        
        // 2. Apply the frame slicing math
        vec2 final_uv = (flipped_uv * uv_scale) + uv_offset;
        
        // 3. Sample and draw
        vec4 texColor = texture(image, final_uv);
        if(texColor.a < 0.1) discard; 
        FragColor = texColor * color; 
    }
    else
    {
        // Dash UI (Fills from bottom up)
        if ((1.0 - TexCoords.y) <= fill_amount)
        {
            FragColor = color;
        }
        else
        {
            FragColor = bg_color;
        }
    }
}