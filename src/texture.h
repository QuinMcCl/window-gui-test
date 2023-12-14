#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <map>

class Texture
{
private:
    static std::map<GLint, GLuint> TextureUnitMap;
    GLuint mTextureID;
    GLenum mTextureUnit;
    glm::vec2 mSize;

public:
    Texture(std::string filePath);
    GLuint getID();
    glm::vec2 getSize();
    GLint activate();
    void cleanup();
};

class TextureManager
{
public:
    void cleanup();
    Texture * getTexture(std::string filePath);
    unsigned int deleteTexture(std::string filePath);

private:
    std::map<std::string, Texture> allTextures;
};

#endif