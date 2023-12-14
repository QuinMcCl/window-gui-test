#include <GL/glew.h>
#include <stb/stb_image.h>
#include <map>

#include "texture.h"
std::map<GLint, GLuint> Texture::TextureUnitMap;

Texture::Texture(std::string filePath) : mTextureID(GL_FALSE), mTextureUnit(GL_FALSE), mSize(glm::ivec2(-1, -1))
{

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {

        glGenTextures(1, &mTextureID);
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        mSize = glm::ivec2(width, height);

        glBindTexture(GL_TEXTURE_2D, mTextureID);
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
}

unsigned int Texture::getID()
{
    return mTextureID;
}
glm::vec2 Texture::getSize()
{
    return mSize;
}

GLint Texture::activate()
{
    if (mTextureUnit == GL_FALSE)
    {
        GLint MaxTextureImageUnits;
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &MaxTextureImageUnits); // fragment
        //  glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &MaxVertexTextureImageUnits); // vertex
        //  glGetIntegerv(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, &MaxGSGeometryTextureImageUnits); // geometry
        GLint unitID = 0;
        for (; unitID < MaxTextureImageUnits; unitID++)
        {
            if (TextureUnitMap.find(unitID) == TextureUnitMap.end())
                break;
        }
        if (unitID == MaxTextureImageUnits)
        {
            // trouble max textures loaded
        }
        mTextureUnit = unitID;
        glActiveTexture(GL_TEXTURE0 + mTextureUnit);
        glBindTexture(GL_TEXTURE_2D, mTextureID);
        TextureUnitMap.insert(std::pair<GLint, GLuint>(mTextureUnit, mTextureID));
    }
    return mTextureUnit;
}

void Texture::cleanup()
{
    glDeleteTextures(1, &mTextureID);
}

Texture *TextureManager::getTexture(std::string filePath)
{
    std::map<std::string, Texture>::iterator it = allTextures.find(filePath);
    if (it == allTextures.end())
    {
        Texture theTexture = Texture(filePath);
        if (theTexture.getID() != GL_FALSE)
        {
            std::pair<std::map<std::string, Texture>::iterator, bool> result = allTextures.insert(std::map<std::string, Texture>::value_type(filePath, theTexture));
            if (result.second)
            {
                it = result.first;
            }
        }
    }
    return &(it->second);
}

unsigned int TextureManager::deleteTexture(std::string filePath)
{
    std::map<std::string, Texture>::iterator it = allTextures.find(filePath);
    if (it != allTextures.end())
    {
        it->second.cleanup();
        allTextures.erase(it);
        return GL_TRUE;
    }
    return GL_FALSE;
}

void TextureManager::cleanup()
{
    for (std::map<std::string, Texture>::iterator it = allTextures.begin(); it != allTextures.end();)
    {
        it->second.cleanup();
        allTextures.erase(it++);
    }
}