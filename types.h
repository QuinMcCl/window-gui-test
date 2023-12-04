#ifndef TYPES_H
#define TYPES_H
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define MAX_NAME_LENGTH 255
#define MAX_PATH_LENGTH 255
#define MAX_BONE_INFLUENCE 4
#define MAX_TEXTURECOORDS 8

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords[MAX_TEXTURECOORDS];
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture
{
    unsigned int id = 0;
    unsigned int type = 0;
    const char *path = NULL;
};

#endif