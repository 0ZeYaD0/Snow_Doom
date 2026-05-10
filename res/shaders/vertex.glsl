#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexUV;
layout (location = 3) in vec3 aColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 vertexColor;
out vec3 normal;
out vec2 TexCoord;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    
    vertexColor = aColor;
    TexCoords = aTexUV; // Pass the UV coordinates right through
    
    TexCoord = aTexUV;
    
    normal = mat3(transpose(inverse(model))) * aNormal; 
    // This math ensures the normals point the right way even if you rotate or scale the model
    normal = mat3(transpose(inverse(model))) * aNormal;
}