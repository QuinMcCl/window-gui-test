#ifndef GLFW_ENABLED_H
#define GLFW_ENABLED_H
#include <algorithm>
#include <vector>
#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>

#include <string>

class glfw_enabled
{
private:
    std::vector<glfw_enabled *> children;

public:
    glfw_enabled()
    {
    }
    ~glfw_enabled()
    {
        this->children.clear();
    }

    virtual void adopt(glfw_enabled *child)
    {
        this->children.push_back(child);
    }

    virtual glfw_enabled *orphan(glfw_enabled *child)
    {
        std::vector<glfw_enabled *>::iterator it = std::find(this->children.begin(), this->children.end(), child);
        if (it != this->children.end())
        {
            children.erase(it);
            return child;
        }
        return NULL;
    }

    virtual void update(float dt)
    {
        for (std::vector<glfw_enabled *>::iterator c = this->children.begin(); c != this->children.end(); ++c)
            (*c)->update(dt);
    }

    virtual void draw()
    {
        for (std::vector<glfw_enabled *>::iterator c = this->children.begin(); c != this->children.end(); ++c)
            (*c)->draw();
    }

    virtual void charfun(unsigned int codepoint)
    {
        for (std::vector<glfw_enabled *>::iterator c = this->children.begin(); c != this->children.end(); ++c)
            (*c)->charfun(codepoint);
    }
    virtual void charmodsfun(unsigned int codepoint, int mods)
    {
        for (std::vector<glfw_enabled *>::iterator c = this->children.begin(); c != this->children.end(); ++c)
            (*c)->charmodsfun(codepoint, mods);
    }
    virtual void cursorenterfun(int entered)
    {
        for (std::vector<glfw_enabled *>::iterator c = this->children.begin(); c != this->children.end(); ++c)
            (*c)->cursorenterfun(entered);
    }
    virtual void cursorposfun(double xpos, double ypos)
    {
        for (std::vector<glfw_enabled *>::iterator c = this->children.begin(); c != this->children.end(); ++c)
            (*c)->cursorposfun(xpos, ypos);
    }
    virtual void dropfun(int path_count, const char **paths)
    {
        for (std::vector<glfw_enabled *>::iterator c = this->children.begin(); c != this->children.end(); ++c)
            (*c)->dropfun(path_count, paths);
    }
    virtual void framebuffersizefun(int width, int height)
    {
        for (std::vector<glfw_enabled *>::iterator c = this->children.begin(); c != this->children.end(); ++c)
            (*c)->framebuffersizefun(width, height);
    }
    virtual void joystickfun(int jid, int event)
    {
        for (std::vector<glfw_enabled *>::iterator c = this->children.begin(); c != this->children.end(); ++c)
            (*c)->joystickfun(jid, event);
    }
    virtual void keyfun(int key, int scancode, int action, int mods)
    {
        for (std::vector<glfw_enabled *>::iterator c = this->children.begin(); c != this->children.end(); ++c)
            (*c)->keyfun(key, scancode, action, mods);
    }
    virtual void monitorfun(GLFWmonitor *monitor, int event)
    {
        for (std::vector<glfw_enabled *>::iterator c = this->children.begin(); c != this->children.end(); ++c)
            (*c)->monitorfun(monitor, event);
    }
    virtual void mousebuttonfun(int button, int action, int mods)
    {
        for (std::vector<glfw_enabled *>::iterator c = this->children.begin(); c != this->children.end(); ++c)
            (*c)->mousebuttonfun(button, action, mods);
    }
    virtual void scrollfun(double xoffset, double yoffset)
    {
        for (std::vector<glfw_enabled *>::iterator c = this->children.begin(); c != this->children.end(); ++c)
            (*c)->scrollfun(xoffset, yoffset);
    }
    virtual void closefun()
    {
        for (std::vector<glfw_enabled *>::iterator c = this->children.begin(); c != this->children.end(); ++c)
            (*c)->closefun();
    }
    virtual void contentscalefun(float xscale, float yscale)
    {
        for (std::vector<glfw_enabled *>::iterator c = this->children.begin(); c != this->children.end(); ++c)
            (*c)->contentscalefun(xscale, yscale);
    }
    virtual void focusfun(int focused)
    {
        for (std::vector<glfw_enabled *>::iterator c = this->children.begin(); c != this->children.end(); ++c)
            (*c)->focusfun(focused);
    }
    virtual void iconifyfun(int iconified)
    {
        for (std::vector<glfw_enabled *>::iterator c = this->children.begin(); c != this->children.end(); ++c)
            (*c)->iconifyfun(iconified);
    }
    virtual void maximizefun(int maximized)
    {
        for (std::vector<glfw_enabled *>::iterator c = this->children.begin(); c != this->children.end(); ++c)
            (*c)->maximizefun(maximized);
    }
    virtual void posfun(int xpos, int ypos)
    {
        for (std::vector<glfw_enabled *>::iterator c = this->children.begin(); c != this->children.end(); ++c)
            (*c)->posfun(xpos, ypos);
    }
    virtual void refreshfun()
    {
        for (std::vector<glfw_enabled *>::iterator c = this->children.begin(); c != this->children.end(); ++c)
            (*c)->refreshfun();
    }
    virtual void sizefun(int width, int height)
    {
        for (std::vector<glfw_enabled *>::iterator c = this->children.begin(); c != this->children.end(); ++c)
            (*c)->sizefun(width, height);
    }
};

class uniformBufferedObject
{
private:
    static unsigned int mAllBindingPoints;
    unsigned int mBindingPoint = GL_FALSE;
    unsigned int uboBlock = GL_FALSE;
    std::string mName = " ";

public:
    uniformBufferedObject(const std::string name, const unsigned int maxSize)
    {
        mBindingPoint = mAllBindingPoints++;
        mName = name;
        // allocate buffer
        glGenBuffers(1, &uboBlock);
        glBindBuffer(GL_UNIFORM_BUFFER, uboBlock);
        glBufferData(GL_UNIFORM_BUFFER, maxSize, NULL, GL_STATIC_DRAW);

        // bind the buffer
        glBindBufferBase(GL_UNIFORM_BUFFER, mBindingPoint, uboBlock);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    template <typename T>
    void fill(const std::vector<T> items)
    {
        // fill the buffer
        glBindBuffer(GL_UNIFORM_BUFFER, uboBlock);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, items.size() * sizeof(T), &items[0]);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    ~uniformBufferedObject()
    {
        glDeleteBuffers(1, &uboBlock);
        uboBlock = GL_FALSE;
    }
    std::string getName()
    {
        return mName;
    }
    unsigned int getBlockBindingIndex()
    {
        return mBindingPoint;
    }
};

#endif