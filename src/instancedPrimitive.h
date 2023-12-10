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
    InstancedPrimitive(primitive_type type, const Shader * shader, const Texture * image);
    ~InstancedPrimitive();
    void updateMatricies(unsigned int modelCount, glm::mat4 * model, glm::mat4  mView, glm::mat4 mProjection);
    void draw() override;
private:
    unsigned int mModelCount;

};

#endif