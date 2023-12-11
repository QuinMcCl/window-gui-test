#ifndef FONTS_H
#define FONTS_H

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
#include "glfw_enabled.h"

struct letterStruct
{
    glm::vec2 charSize;
    glm::vec2 charBearing;
    glm::vec2 textSize;
    glm::vec2 textBearing;
};

struct charStruct
{
    unsigned int index;
    unsigned int pad1;
    unsigned int pad2;
    unsigned int pad3;
    glm::vec3 Offset;
    unsigned int pad4;
};

class FontRenderer : public glfw_enabled
{
private:
    Texture *mAtlas;

    uniformBufferedObject uboAlphabetBlock;
    std::vector<letterStruct> AlphabetData;

    uniformBufferedObject uboTextBlock;
    std::map<char, charStruct> metaTable;
    unsigned int VBO, VAO;

    const Shader *mShader;
    glm::mat4 mModel, mView, mProjection;
    std::string mText;
    glm::vec4 mColor;

public:
    FontRenderer(const char *metaPath, const Shader *shader, Texture *Atlas);

    void cleanup() override;

    void updateMatricies(glm::mat4 model, glm::mat4 view, glm::mat4 projection);

    void setColorText(glm::vec4 color, std::string text);

    void draw() override;
};

#endif