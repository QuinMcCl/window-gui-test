#ifndef SHADER_H
#define SHADER_H
#include "types.h"


class Shader
{
private:
    unsigned int shaderProgram;
    const char *vertexPath;
    const char *fragmentPath;

public:
    Shader(const char *vP, const char *fP);
    ~Shader();
    int read_and_compile();
    void use();
    unsigned int get_ID();
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setInt(const char *name, int value) const;
    void setFloat(const char *name, float value) const;
    void setVec2(const char *name, const glm::vec2 &value) const;
    void setVec2(const char *name, float x, float y) const;
    void setVec3(const char *name, const glm::vec3 &value) const;
    void setVec3(const char *name, float x, float y, float z) const;
    void setVec4(const char *name, const glm::vec4 &value) const;
    void setVec4(const char *name, float x, float y, float z, float w);
    void setMat2(const char *name, const glm::mat2 &mat) const;
    void setMat3(const char *name, const glm::mat3 &mat) const;
    void setMat4(const char *name, const glm::mat4 &mat) const;
};

#endif