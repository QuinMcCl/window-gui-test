#include "texture.h"
#include "shader.h"

#include "instancedPrimitive.h"

InstancedPrimitive::InstancedPrimitive(primitive_type type, Shader *shader, Texture *image) : Primitive(type, shader, image)
{

}

void InstancedPrimitive::updateModels(unsigned int modelCount, glm::mat4 models[])
{
    mModels = models;
    mModelCount = modelCount;
}

void InstancedPrimitive::draw()
{
    // activate shader
    mShader->use();
    mShader->setMat4("model", mModelCount, mModels);
    mShader->setInt("texture1", mImage->activate());

    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, mIndeces.size(), GL_UNSIGNED_INT, 0, mModelCount);
    glBindVertexArray(0);
}