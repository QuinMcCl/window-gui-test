/*
TODO:
use freetype to load font (TTF, Opentype, etc) and render SDF to atlas
save atlas


*/
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

#include "shader.h"
#include "glfw_enabled.h"

struct Character
{
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2 Size;        // Size of glyph
    glm::ivec2 Bearing;     // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Offset to advance to next glyph
};
class Font : public glfw_enabled
{
public:
    std::map<char, Character> Characters;
    unsigned int VAO, VBO;
    Shader * mShader;
    std::string mText;
    glm::vec3 mPos;


    Font(const char *filepathname, Shader * shader)
    {
        mShader = shader;
        FT_Library ft;
        if (FT_Init_FreeType(&ft))
        {
            fprintf(stderr, "ERROR::FREETYPE: Could not init FreeType Library\n");
            return;
        }

        FT_Face face;
        if (FT_New_Face(ft, filepathname, 0, &face))
        {
            fprintf(stderr, "ERROR::FREETYPE: Failed to load font\n");
            return;
        }

        FT_Error error = FT_Set_Pixel_Sizes(
            face, /* handle to face object */
            0,    /* pixel_width           */
            32);  /* pixel_height          */

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

        FT_GlyphSlot slot = face->glyph; // <-- This is new

        unsigned int numGlyph = face->num_glyphs;

        for (unsigned char c = 32; c < 127; c++)
        {
            // load character glyph
            if (FT_Load_Char(face, c, FT_LOAD_DEFAULT))
            {
                fprintf(stderr, "ERROR::FREETYPE: Failed to load Glyph\n");
                continue;
            }

            // fprintf(stderr, "\"%d\",\t\"%c\",\t%d,\t%d,\t%d,\t%d,\t%ld,\t%ld\n", c, c, face->glyph->bitmap.rows, face->glyph->bitmap.width, face->glyph->bitmap_left, face->glyph->bitmap_top, face->glyph->advance.x, face->glyph->advance.y);

            if (FT_Render_Glyph(slot, FT_RENDER_MODE_SDF))
            {
                fprintf(stderr, "ERROR::FREETYPE: Failed to render Glyph\n");
                continue;
            }

            // generate texture
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer);
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // now store character for later use
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                face->glyph->advance.x};
            Characters.insert(std::pair<char, Character>(c, character));
        }
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 5, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void * )(3 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    
    void setTextPos(std::string text, glm::vec3 pos){
        mText = text;
        mPos = pos;
    }


    void draw()
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE); // Don't write into the depth buffer
        glDisable(GL_CULL_FACE);
        // activate corresponding render state
        mShader->use();

        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(VAO);

        float x = mPos.x;
        float y = mPos.y;
        float scale = mPos.z;


        // iterate through all characters
        std::string::const_iterator c;
        for (c = mText.begin(); c != mText.end(); c++)
        {
            Character ch = Characters[*c];

            float xpos = x + ch.Bearing.x * scale;
            float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;
            // update VBO for each character
            float vertices[6][5] = {
                {xpos, ypos + h, 0.0f,0.0f, 0.0f},
                {xpos, ypos, 0.0f,0.0f, 1.0f},
                {xpos + w, ypos, 0.0f,1.0f, 1.0f},

                {xpos, ypos + h, 0.0f,0.0f, 0.0f},
                {xpos + w, ypos, 0.0f,1.0f, 1.0f},
                {xpos + w, ypos + h, 0.0f,1.0f, 0.0f}};
            // render glyph texture over quad
            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            // update content of VBO memory
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            // render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glEnable(GL_CULL_FACE);
        glDepthMask(GL_TRUE); // Re-enable writing to the depth buffer
        glDisable(GL_BLEND);
    }
};

#endif