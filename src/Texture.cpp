#include "Texture.h"

#include <iostream>
#include <glad/glad.h>
#include <stb_image.h>

Texture::Texture(const char * texturePath)
{
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);

    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load image, create texture and generate mipmaps
    int width, height, nfChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char *data = stbi_load(texturePath, &width, &height, &nfChannels, 0);
    
    if (data)
    {
        GLenum format = 0;
        if (nfChannels == 1)
            format = GL_RED;
        else if (nfChannels == 3)
            format = GL_RGB;
        else if (nfChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << texturePath << std::endl;
    }

    stbi_image_free(data);
}

void Texture::use()
{
    glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ID);
}