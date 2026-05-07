#include <engine/graphics/texture.h>
#include <stb/stb_image.h>
#include <glad/glad.h>
#include <engine/core/defines.h>
#include <iostream>

Texture::Texture(const string &filepath)
{
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);

    // Texture wrapping/filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Use nearest neighbor filtering for that crunchy retro DOOM pixel art look!
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // OpenGL expects 0.0 on the Y-axis to be the bottom, images usually have it at the top
    stbi_set_flip_vertically_on_load(true);

    unsigned char *data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);
    if (data)
    {
        GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture: " << filepath << std::endl;
    }
    stbi_image_free(data);
}

Texture::~Texture()
{
    glDeleteTextures(1, &ID);
}

void Texture::Bind(unsigned int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::Unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}