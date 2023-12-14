#ifndef PRIMITIVE_H
#define PRIMITIVE_H
#include <vector>
#include "shader.h"
#include "texture.h"
#include "glfw_enabled.h"
#include "types.h"

enum primitive_type
{
    TRIANGLE,
    SQUARE,
    CUBE
};

class Primitive : public glfw_enabled
{
public:
    Primitive(primitive_type type, Shader *shader, Texture *image);
    void cleanup();
    void updateModel(glm::mat4 mModel);
    void draw() override;

protected:
    Shader *mShader;
    Texture *mImage;
    glm::mat4 mModel;
    glm::mat4 mView;
    glm::mat4 mProjection;
    unsigned int VAO;
    std::vector<unsigned int> mIndeces;
    std::vector<Vertex> mVertices;

private:
    unsigned int VBO, EBO;
};

#endif