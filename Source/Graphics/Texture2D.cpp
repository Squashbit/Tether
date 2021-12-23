#include <Link/Graphics/Texture2D.hpp>
#include <Link/Graphics/OpenGL.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace Link::Graphics;

bool Texture2D::Load(uint8_t* data, uint64_t width, uint64_t height, 
	int channels)
{
    if (initialized || !data || width == 0 || height == 0 || channels == 0)
        return false;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    int glChannels = GL_RGB;
    if (channels == 4)
        glChannels = GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, glChannels, width, height, 0, glChannels, 
        GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    initialized = true;
    return true;
}

bool Texture2D::Load(std::string filePath)
{
    if (initialized)
        return false;
    
    stbi_set_flip_vertically_on_load(true);
    int width, height, channels;
    uint8_t* data = stbi_load(filePath.c_str(), &width, &height, 
        &channels, 0);
    if (!data)
    {
        stbi_image_free(data);
        return false;
    }

    if (!Load(data, width, height, channels))
        return false;
    
    stbi_image_free(data);

    initialized = true;
    return true;
}

void Texture2D::TexParameteri(uint32_t pname, int param)
{
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, pname, param);
}

void Texture2D::Bind(uint8_t activeTexture)
{
    if (activeTexture > 31)
        return;
    
    glActiveTexture(GL_TEXTURE0 + activeTexture);
    glBindTexture(GL_TEXTURE_2D, texture);
}

void Texture2D::OnDispose()
{
    glDeleteTextures(1, &texture);
}