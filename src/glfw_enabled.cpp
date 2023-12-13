#include "glfw_enabled.h"

unsigned int uniformBufferedObject::mAllBindingPoints = 0;

uniformBufferedObject::uniformBufferedObject(const std::string name, const unsigned int maxSize)
    : mBindingPoint(mAllBindingPoints++), mName(name), mMaxSize(maxSize)
{
    // allocate buffer
    glGenBuffers(1, &uboBlock);
    glBindBuffer(GL_UNIFORM_BUFFER, uboBlock);
    glBufferData(GL_UNIFORM_BUFFER, mMaxSize, NULL, GL_STATIC_DRAW);

    // bind the buffer
    glBindBufferBase(GL_UNIFORM_BUFFER, mBindingPoint, uboBlock);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void uniformBufferedObject::fill(const void *items, unsigned int size, unsigned int offset)
{
    if (size + offset > mMaxSize)
    {
        glDeleteBuffers(1, &uboBlock);
        uboBlock = GL_FALSE;
        mMaxSize = size + offset;
        glGenBuffers(1, &uboBlock);
        glBindBuffer(GL_UNIFORM_BUFFER, uboBlock);
        glBufferData(GL_UNIFORM_BUFFER, mMaxSize, NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_UNIFORM_BUFFER, offset, size, items);
        glBindBufferBase(GL_UNIFORM_BUFFER, mBindingPoint, uboBlock);
    }
    else
    {
        // fill the buffer
        glBindBuffer(GL_UNIFORM_BUFFER, uboBlock);
        glBufferSubData(GL_UNIFORM_BUFFER, offset, size, items);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
}

void uniformBufferedObject::cleanup()
{
    glDeleteBuffers(1, &uboBlock);
    uboBlock = GL_FALSE;
}
std::string uniformBufferedObject::getName()
{
    return mName;
}
unsigned int uniformBufferedObject::getBlockBindingIndex()
{
    return mBindingPoint;
}