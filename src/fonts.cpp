#include "fonts.h"

FontRenderer::FontRenderer(const char *metaPath, const Shader *shader, Texture *Atlas) : uboAlphabetBlock("uboAlphabetBlock", 96 * sizeof(letterStruct)),
                                                                                         uboTextBlock("uboTextBlock", 256 * sizeof(charStruct))
{
    mShader = shader;
    mAtlas = Atlas;

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

            letterStruct line;
            charStruct thisChar;

            thisChar.index = AlphabetData.size();
            thisChar.Offset = glm::vec3(advance, 0.0, 0.0);

            line.charBearing = glm::vec2(charLeft, 1.0f -charTop);
            line.charSize = glm::vec2(charRight - charLeft, charTop - charBottom);
            line.textBearing = glm::vec2(atlasLeft, atlasBottom) / mAtlas->size;
            line.textSize = glm::vec2(atlasRight - atlasLeft, atlasTop - atlasBottom) / mAtlas->size;

            metaTable.insert(std::pair<char, charStruct>(c, thisChar));
            AlphabetData.push_back(line);
        }

        fclose(fp);
    }

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

    // fill the buffer
    uboAlphabetBlock.fill<letterStruct>(AlphabetData);
}

void FontRenderer::cleanup()
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
    glfw_enabled::cleanup();
}

void FontRenderer::updateMatricies(glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
    mModel = model;
    mView = view;
    mProjection = projection;
}

void FontRenderer::setColorText(glm::vec4 color, std::string text)
{
    mText = text;
    mColor = color;
    std::vector<charStruct> TextData;
    glm::vec3 totalOffset = glm::vec3(0.0, 0.0, 0.0);

    for (std::string::iterator it = text.begin(); it != text.end(); it++)
    {
        if(*it == '\n')
        {
            totalOffset.x = 0;
            totalOffset.y += 1;
        }
        else
        {
            std::map<char, charStruct>::iterator tableIt = metaTable.find(*it);
            if (tableIt != metaTable.end())
            {
                charStruct foundChar = tableIt->second;
                glm::vec3 addOffset = foundChar.Offset;
                foundChar.Offset = totalOffset;
                TextData.push_back(foundChar);
                totalOffset += addOffset;
            }
        }
        
    }
    uboTextBlock.fill(TextData);
}

void FontRenderer::draw()
{
    mShader->use();
    
    mShader->bindBuffer(uboAlphabetBlock.getName().c_str(), uboAlphabetBlock.getBlockBindingIndex());
    mShader->bindBuffer(uboTextBlock.getName().c_str(), uboTextBlock.getBlockBindingIndex());
    
    mShader->setMat4("projection", mProjection);
    mShader->setMat4("view", mView);
    mShader->setMat4("model", mModel);
    mShader->setVec4("TextColor", mColor);

    // glm::vec3 offset = glm::vec3(0.0, 0.0, 0.0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE); // Don't write into the depth buffer
    // glDisable(GL_CULL_FACE);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mAtlas->id);
    glBindVertexArray(VAO);

    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, mText.size());

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // glEnable(GL_CULL_FACE);
    glDepthMask(GL_TRUE); // Re-enable writing to the depth buffer
    glDisable(GL_BLEND);
}