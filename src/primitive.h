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
    Primitive(primitive_type type, const Shader * shader, const Texture * image);
    ~Primitive();
    void updateMatricies(glm::mat4 mModel, glm::mat4  mView, glm::mat4 mProjection);
    void draw() override;
private:
    const Shader * mShader;
    const Texture * mImage;
    glm::mat4 mModel, mView, mProjection;
    std::vector<Vertex> mVertices;
    std::vector<unsigned int> mIndeces;
    unsigned int VAO, VBO, EBO;

};

#endif