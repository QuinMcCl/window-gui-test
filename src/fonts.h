#ifndef FONTS_H
#define FONTS_H

#define MAX_ALPHABETS 16
#define LETTERS_PER_ALPHABET 128


#include <ft2build.h>
#include FT_FREETYPE_H

#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <map>
#include <string>
#include <stdio.h>

#include "texture.h"
#include "shader.h"
#include "gl_ubo.h"
#include "glfw_enabled.h"

struct letterStruct
{
    glm::vec2 ScreenSize;
    glm::vec2 ScreenBearing;
    glm::vec2 UVSize;
    glm::vec2 UVBearing;
};

struct charStruct
{
    int index;
    int pad1;
    int pad2;
    int pad3;
    glm::vec3 Advance;
    int pad4;
};

class FontType
{
private:
    Texture *mAtlas;
    uniformBufferedObject uboAlphabetBlock;
    std::map<char, charStruct> mIndexTable;

public:
    FontType(const char *metaPath, Texture *Atlas);
    Texture *getTexture();
    charStruct getChar(char c);
    uniformBufferedObject getBlock();
    void cleanup();
};

class FontString
{
private:
    const FontType &type;
    uniformBufferedObject uboTextBlock;
    std::vector<charStruct> TextData;

public:
};

class RenderedText : public glfw_enabled
{
private:
    FontType * mType;
    uniformBufferedObject uboTextBlock;
    unsigned int VBO, VAO;

    Shader *mShader;
    glm::mat4 mModel;
    std::string mText;
    glm::vec4 mColor;

public:
    RenderedText(FontType *type, Shader *shader);

    void cleanup() override;

    void updateModel(glm::mat4 model);

    void setColor(glm::vec4 color);
    void setText(std::string text);

    void draw() override;
};

#endif