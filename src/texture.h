#ifndef TEXTURE_H
#define TEXTURE_H

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
    ~TextureManager();
    Texture getTexture(std::string filePath, unsigned int type);
    unsigned int unloadTexture(std::string filePath, unsigned int type);
private:
    std::map<std::string, Texture> allTextures;
    unsigned int TextureFromFile(std::string filePath, bool gamma = false);
};

#endif