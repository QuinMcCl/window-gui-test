#include "fonts.h"

FontType::FontType(const char *metaPath, Texture *Atlas) : mAtlas(Atlas),
                                                           uboAlphabetBlock("uboAlphabetBlock", LETTERS_PER_ALPHABET * sizeof(letterStruct)),
                                                           mIndexTable()
{
    std::vector<letterStruct> mAlphabetData;

    // parse csv
    FILE *fp = fopen(metaPath, "rb");

    if (fp == NULL)
    {
        perror("Error opening file");
        return;
    }
    else
    {
        char buffer[256];
        while (fgets(buffer, 256, fp) != NULL)
        {
            char *pch = strtok(buffer, ",");
            if (pch == NULL)
                return;
            char c = (char)atoi(pch);

            pch = strtok(NULL, ",");
            if (pch == NULL)
                return;
            double advance = atof(pch);

            pch = strtok(NULL, ",");
            if (pch == NULL)
                return;
            double charLeft = atof(pch);

            pch = strtok(NULL, ",");
            if (pch == NULL)
                return;
            double charBottom = atof(pch);

            pch = strtok(NULL, ",");
            if (pch == NULL)
                return;
            double charRight = atof(pch);

            pch = strtok(NULL, ",");
            if (pch == NULL)
                return;
            double charTop = atof(pch);

            pch = strtok(NULL, ",");
            if (pch == NULL)
                return;
            double atlasLeft = atof(pch);

            pch = strtok(NULL, ",");
            if (pch == NULL)
                return;
            double atlasBottom = atof(pch);

            pch = strtok(NULL, ",");
            if (pch == NULL)
                return;
            double atlasRight = atof(pch);

            pch = strtok(NULL, ",");
            if (pch == NULL)
                return;
            double atlasTop = atof(pch);

            pch = strtok(NULL, ",");
            if (pch != NULL)
                return;

            letterStruct Glyph;
            charStruct charData;

            charData.index = mAlphabetData.size();
            charData.Advance = glm::vec3(advance, 0.0f, 0.0f);

            Glyph.ScreenBearing = glm::vec2(charLeft, 1.0f - charTop);
            Glyph.ScreenSize = glm::vec2(charRight - charLeft, charTop - charBottom);
            Glyph.UVBearing = glm::vec2(atlasLeft, atlasBottom) / mAtlas->getSize();
            Glyph.UVSize = glm::vec2(atlasRight - atlasLeft, atlasTop - atlasBottom) / mAtlas->getSize();

            mIndexTable.insert(std::pair<char, charStruct>(c, charData));
            mAlphabetData.push_back(Glyph);
        }

        fclose(fp);
    }
    // fill the buffer
    uboAlphabetBlock.fill(&mAlphabetData[0], mAlphabetData.size() * sizeof(letterStruct),0);
}

Texture *FontType::getTexture()
{
    return mAtlas;
}

charStruct FontType::getChar(char c)
{
    std::map<char, charStruct>::iterator tableIt = mIndexTable.find(c);
    if (tableIt != mIndexTable.end())
    {
        return tableIt->second;
    }
    return (charStruct){.index = -1};
}

uniformBufferedObject FontType::getBlock()
{
    return uboAlphabetBlock;
}

void FontType::cleanup(){
    uboAlphabetBlock.cleanup();
}

RenderedText::RenderedText(FontType *type, Shader *shader) : uboTextBlock("uboTextBlock", 2048 * sizeof(charStruct)),
                                                             mType(type),
                                                             mShader(shader)
{
    float vertices[6][4] = {
        {0.0f, 0.0f, 0.0f, 1.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {1.0f, 1.0f, 1.0f, 0.0f},

        {1.0f, 1.0f, 1.0f, 0.0f},
        {1.0f, 0.0f, 1.0f, 1.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}};

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void RenderedText::cleanup()
{
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    if (VAO != GL_FALSE)
    {
        glDeleteVertexArrays(1, &VAO);
        VAO = GL_FALSE;
    }
    if (VBO != GL_FALSE)
    {
        glDeleteBuffers(1, &VBO);
        VBO = GL_FALSE;
    }
    uboTextBlock.cleanup();
    glfw_enabled::cleanup();
}

void RenderedText::updateMatricies(glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
    mModel = model;
    mView = view;
    mProjection = projection;
}

void RenderedText::setColorText(glm::vec4 color, std::string text)
{
    mText = text;
    mColor = color;
    std::vector<charStruct> TextData;
    glm::vec3 CurrentAdvance = glm::vec3(0.0, 0.0, 0.0);
    glm::vec3 AddAdvance = glm::vec3(0.0, 0.0, 0.0);
    charStruct thisChar;
    thisChar.index = -1;
    thisChar.Advance = glm::vec3(0.0, 0.0, 0.0);

    for (std::string::iterator it = text.begin(); it != text.end(); it++)
    {
        if (*it == '\n')
        {
            CurrentAdvance.x = 0;
            CurrentAdvance.y += 1;
        }
        else
        {

            thisChar = mType->getChar(*it);
            if (thisChar.index != -1)
            {
                AddAdvance = thisChar.Advance;
                thisChar.Advance = CurrentAdvance;
                TextData.push_back(thisChar);
                CurrentAdvance += AddAdvance;
            }
        }
    }
    uboTextBlock.fill(&TextData[0], TextData.size() * sizeof(charStruct), 0);
}

void RenderedText::draw()
{
    mShader->use();

    mShader->setMat4("projection", mProjection);
    mShader->setMat4("view", mView);
    mShader->setMat4("model", mModel);
    mShader->setVec4("TextColor", mColor);
    mShader->setInt("texture1", mType->getTexture()->activate());
    mShader->bindBuffer(mType->getBlock().getName().c_str(), mType->getBlock().getBlockBindingIndex());
    mShader->bindBuffer(uboTextBlock.getName().c_str(), uboTextBlock.getBlockBindingIndex());

    // glm::vec3 offset = glm::vec3(0.0, 0.0, 0.0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE); // Don't write into the depth buffer
    // glDisable(GL_CULL_FACE);

    glBindVertexArray(VAO);

    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, mText.size());

    glBindVertexArray(0);

    // glEnable(GL_CULL_FACE);
    glDepthMask(GL_TRUE); // Re-enable writing to the depth buffer
    glDisable(GL_BLEND);
}