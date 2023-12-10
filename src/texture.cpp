#include <GL/glew.h>
#include <stb/stb_image.h>
#include <map>

#include "texture.h"

TextureManager::~TextureManager()
{
    Texture theTexture;
    for (std::map<std::string, Texture>::iterator it = allTextures.begin(); it != allTextures.end();)
    {
        theTexture = it->second;
        allTextures.erase(it++);
        glDeleteTextures(1, &(theTexture.id));
    }
}

Texture TextureManager::TextureFromFile(std::string filePath, bool gamma)
{
    Texture theTexture = {GL_FALSE, glm::ivec2(0, 0)};
    unsigned int textureID = GL_FALSE;

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {

        glGenTextures(1, &textureID);
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        theTexture.id = textureID;
        theTexture.size = glm::ivec2(width, height);

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        fprintf(stderr, "\n", filePath);
        stbi_image_free(data);
    }

    return theTexture;
}

Texture TextureManager::getTexture(std::string filePath, unsigned int type)
{
    std::map<std::string, Texture>::iterator it = allTextures.find(filePath);
    if (it == allTextures.end())
    {
        Texture theTexture = TextureFromFile(filePath);
        if (theTexture.id != GL_FALSE)
        {
            std::pair<std::map<std::string, Texture>::iterator, bool> result = allTextures.insert(std::map<std::string, Texture>::value_type(filePath, theTexture));
            if (result.second)
            {
                it = result.first;
            }
        }
    }
    return it->second;
}

unsigned int TextureManager::unloadTexture(std::string filePath, unsigned int type)
{
    Texture theTexture;
    std::map<std::string, Texture>::iterator it = allTextures.find(filePath);
    if (it != allTextures.end())
    {
        theTexture = it->second;
        allTextures.erase(it);
        glDeleteTextures(1, &(theTexture.id));
        return GL_TRUE;
    }
    return GL_FALSE;
}
