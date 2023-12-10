#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>

#include "shader.h"

char * readFile(const char* filename){
  std::FILE *fp = std::fopen(filename, "rb");
  if (fp != NULL)
  {
    std::fseek(fp, 0, SEEK_END);
    long fileSize = std::ftell(fp);
    std::fseek(fp, 0, SEEK_SET);

    char * contents = new char[fileSize+1];
    contents[fileSize] = '\0';
    std::rewind(fp);
    std::fread(contents, fileSize, 1, fp);
    std::fclose(fp);
    return(contents);
  }
  throw(errno);
}

unsigned int CompileShader(unsigned int type, const char *shaderCode){
    unsigned int id = GL_FALSE;
    GLint success = GL_FALSE;
    GLchar infoLog[1024] = "";
    
    id = glCreateShader(type);
    glShaderSource(id, 1, &shaderCode, NULL);
    glCompileShader(id);
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(id, 1024, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER_COMPILATION_ERROR::%s\n", infoLog);
        return GL_FALSE;
    }
    return id;
}

unsigned int linkShader(unsigned int vertex, unsigned int fragment){
    unsigned int id = GL_FALSE;
    GLint success = GL_FALSE;
    GLchar infoLog[1024] = "";

    id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(id, 1024, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER_COMPILATION_ERROR::LINKING::%s\n", infoLog);
        return GL_FALSE;
    }
    return id;
}


Shader::Shader(const char *vertexPath, const char *fragmentPath)
{
    ID = GL_FALSE;
    // 1. retrieve the vertex/fragment source code from filePath
    const char * vertexCode;
    const char * fragmentCode;
    try
    {
        vertexCode = readFile(vertexPath);
        fragmentCode = readFile(fragmentPath);

    }
    catch (std::exception &e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        return;
    }

    // 2. compile shaders
    unsigned int vertex, fragment;
    vertex = CompileShader(GL_VERTEX_SHADER, vertexCode);
    fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentCode);
    delete[] vertexCode;
    delete[] fragmentCode;

    // 3. link shaders
    if(vertex != GL_FALSE && fragment != GL_FALSE)
        ID = linkShader(vertex, fragment);

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);

}

Shader::~Shader(){
    glDeleteShader(ID);
}

void Shader::use() const
{
    glUseProgram(ID);
}
// utility uniform functions
// ------------------------------------------------------------------------
void Shader::setInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::setFloat(const std::string &name, float x) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), x);
}
void Shader::setFloat2(const std::string &name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}
void Shader::setFloat3(const std::string &name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}
void Shader::setFloat4(const std::string &name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}
// ------------------------------------------------------------------------
void Shader::setVec2(const std::string &name, const glm::vec2 &value) const
{
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
{
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
// ------------------------------------------------------------------------
void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void Shader::setMat4(const std::string &name, const unsigned int count, const glm::mat4 mat[]) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), count, GL_FALSE, (const GLfloat *)mat);
}

void Shader::bindBuffer(const char * name, unsigned int BindingIndex) const {
    glUniformBlockBinding(ID, glGetUniformBlockIndex(ID, name), BindingIndex);
}