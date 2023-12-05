#ifndef TYPES_H
#define TYPES_H

#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#define MAX_PATH_LENGTH 255
#define MAX_BONE_INFLUENCE 4


struct Vertex {
    glm::vec3 Position;
    // glm::vec3 Normal;
    glm::vec2 TexCoords;
    // glm::vec3 Tangent;
    // glm::vec3 Bitangent;
	// int m_BoneIDs[MAX_BONE_INFLUENCE];
	// float m_Weights[MAX_BONE_INFLUENCE];
};

#endif