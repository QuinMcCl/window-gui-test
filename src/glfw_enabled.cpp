#include "glfw_enabled.h"

void glfw_enabled::adopt(glfw_enabled *child)
{
    this->children.push_back(child);
}

glfw_enabled *glfw_enabled::orphan(glfw_enabled *child)
{
    std::vector<glfw_enabled *>::iterator it = std::find(this->children.begin(), this->children.end(), child);
    if (it != this->children.end())
    {
        children.erase(it);
        return child;
    }
    return NULL;
}

void glfw_enabled::cleanup()
{
    for (std::vector<glfw_enabled *>::iterator c = this->children.begin(); c != this->children.end(); ++c)
        (*c)->cleanup();
}

void glfw_enabled::update(float dt)
{
    for (std::vector<glfw_enabled *>::iterator c = this->children.begin(); c != this->children.end(); ++c)
        (*c)->update(dt);
}

void glfw_enabled::draw()
{
    for (std::vector<glfw_enabled *>::iterator c = this->children.begin(); c != this->children.end(); ++c)
        (*c)->draw();
}

bool glfw_enabled::handleEvent(GLFW_EVENT event)
{
    bool stop = false;
    for (std::vector<glfw_enabled *>::iterator c = this->children.begin(); !stop && c != this->children.end(); ++c)
        stop |= (*c)->handleEvent(event);
    return stop;
};
