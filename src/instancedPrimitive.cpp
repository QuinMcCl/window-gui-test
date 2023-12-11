#include "texture.h"
#include "shader.h"
#include "types.h"
#include "instancedPrimitive.h"

InstancedPrimitive::InstancedPrimitive(primitive_type type, const Shader *shader, const Texture *image) : Primitive(type, shader, image)
{

}

void InstancedPrimitive::updateMatricies(unsigned int modelCount, glm::mat4 model[], glm::mat4 view, glm::mat4 projection)
{
    mShader->use();
    mShader->setMat4("projection", projection);
    mShader->setMat4("view", view);
    mShader->setMat4("model", modelCount, model);
    mModelCount = modelCount;
}

void InstancedPrimitive::draw()
{
    // activate shader
    mShader->use();
    mShader->setInt("texture1", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mImage->id);

    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, mIndeces.size(), GL_UNSIGNED_INT, 0, mModelCount);
    glBindVertexArray(0);
}