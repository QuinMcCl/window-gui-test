#ifndef GL_UBO_H
#define GL_UBO_H

#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <string>

class uniformBufferedObject
{
private:
    static unsigned int mAllBindingPoints;
    unsigned int mBindingPoint = GL_FALSE;
    unsigned int uboBlock = GL_FALSE;
    unsigned int mMaxSize = GL_FALSE;
    unsigned int mUsage;
    std::string mName = " ";

public:
    uniformBufferedObject(const std::string name, const unsigned int maxSize, unsigned int usage);

    void fill(const void *items, unsigned int size, unsigned int offset);

    void cleanup();

    std::string getName();

    unsigned int getBlockBindingIndex();
};


#endif