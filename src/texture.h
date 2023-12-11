#ifndef TEXTURE_H
#define TEXTURE_H

#include <glm/glm.hpp>
#include <string>
#include <map>

struct Texture
{
    unsigned int id;
    glm::vec2 size;
};

class TextureManager
{
public:
    void cleanup();
    Texture getTexture(std::string filePath, unsigned int type);
    unsigned int unloadTexture(std::string filePath, unsigned int type);
private:
    std::map<std::string, Texture> allTextures;
    Texture TextureFromFile(std::string filePath, bool gamma = false);
};

#endif