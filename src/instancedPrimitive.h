#ifndef INSTANCED_PRIMITIVE_H
#define INSTANCED_PRIMITIVE_H
#include <vector>
#include "shader.h"
#include "texture.h"
#include "glfw_enabled.h"
#include "types.h"
#include "primitive.h"

class InstancedPrimitive : public Primitive
{
public:
    InstancedPrimitive(primitive_type type, Shader * shader, Texture * image);
    void updateMatricies(unsigned int modelCount, glm::mat4 * models, glm::mat4  mView, glm::mat4 mProjection);
    void draw() override;
private:
    glm::mat4 * mModels;
    unsigned int mModelCount;

};

#endif