#ifndef GLFW_ENABLED_H
#define GLFW_ENABLED_H
#include "types.h"

class glfw_enabled
{
private:
    std::vector<glfw_enabled *> children;

public:
    glfw_enabled(){

    }
    ~glfw_enabled(){
        for (std::vector<glfw_enabled *>::iterator c = children.begin(); c != children.end(); ++c)
            delete (*c);
        children.clear();
    }
    void adopt(glfw_enabled * child){
        children.push_back(child);
    }

    void update(float dt){
        for (std::vector<glfw_enabled *>::iterator c = children.begin(); c != children.end(); ++c)
            (*c)->update(dt);
    }

    void draw(){
        for (std::vector<glfw_enabled *>::iterator c = children.begin(); c != children.end(); ++c)
            (*c)->draw();
    }

    void charfun(unsigned int codepoint)
    {
        for (std::vector<glfw_enabled *>::iterator c = children.begin(); c != children.end(); ++c)
            (*c)->charfun(codepoint);
    }
    void charmodsfun(unsigned int codepoint, int mods)
    {
        for (std::vector<glfw_enabled *>::iterator c = children.begin(); c != children.end(); ++c)
            (*c)->charmodsfun(codepoint, mods);
    }
    void cursorenterfun(int entered)
    {
        for (std::vector<glfw_enabled *>::iterator c = children.begin(); c != children.end(); ++c)
            (*c)->cursorenterfun(entered);
    }
    void cursorposfun(double xpos, double ypos) 
    {
        for (std::vector<glfw_enabled *>::iterator c = children.begin(); c != children.end(); ++c)
            (*c)->cursorposfun(xpos, ypos);
    }
    void dropfun(int path_count, const char **paths) 
    {
        for (std::vector<glfw_enabled *>::iterator c = children.begin(); c != children.end(); ++c)
            (*c)->dropfun(path_count, paths);
    }
    void framebuffersizefun(int width, int height) 
    {
        for (std::vector<glfw_enabled *>::iterator c = children.begin(); c != children.end(); ++c)
            (*c)->framebuffersizefun(width, height);
    }
    void joystickfun(int jid, int event) 
    {
        for (std::vector<glfw_enabled *>::iterator c = children.begin(); c != children.end(); ++c)
            (*c)->joystickfun(jid, event);
    }
    void keyfun(int key, int scancode, int action, int mods) 
    {
        for (std::vector<glfw_enabled *>::iterator c = children.begin(); c != children.end(); ++c)
            (*c)->keyfun(key, scancode, action, mods);
    }
    void monitorfun(GLFWmonitor *monitor, int event) 
    {
        for (std::vector<glfw_enabled *>::iterator c = children.begin(); c != children.end(); ++c)
            (*c)->monitorfun(monitor,event);
    }
    void mousebuttonfun(int button, int action, int mods) 
    {
        for (std::vector<glfw_enabled *>::iterator c = children.begin(); c != children.end(); ++c)
            (*c)->mousebuttonfun(button, action, mods);
    }
    void scrollfun(double xoffset, double yoffset) 
    {
        for (std::vector<glfw_enabled *>::iterator c = children.begin(); c != children.end(); ++c)
            (*c)->scrollfun(xoffset, yoffset);
    }
    void closefun() 
    {
        for (std::vector<glfw_enabled *>::iterator c = children.begin(); c != children.end(); ++c)
            (*c)->closefun();
    }
    void contentscalefun(float xscale, float yscale) 
    {
        for (std::vector<glfw_enabled *>::iterator c = children.begin(); c != children.end(); ++c)
            (*c)->contentscalefun(xscale, yscale);
    }
    void focusfun(int focused) 
    {
        for (std::vector<glfw_enabled *>::iterator c = children.begin(); c != children.end(); ++c)
            (*c)->focusfun(focused);
    }
    void iconifyfun(int iconified) 
    {
        for (std::vector<glfw_enabled *>::iterator c = children.begin(); c != children.end(); ++c)
            (*c)->iconifyfun(iconified);
    }
    void maximizefun(int maximized) 
    {
        for (std::vector<glfw_enabled *>::iterator c = children.begin(); c != children.end(); ++c)
            (*c)->maximizefun(maximized);
    }
    void posfun(int xpos, int ypos) 
    {
        for (std::vector<glfw_enabled *>::iterator c = children.begin(); c != children.end(); ++c)
            (*c)->posfun(xpos, ypos);
    }
    void refreshfun() 
    {
        for (std::vector<glfw_enabled *>::iterator c = children.begin(); c != children.end(); ++c)
            (*c)->refreshfun();
    }
    void sizefun(int width, int height) 
    {
        for (std::vector<glfw_enabled *>::iterator c = children.begin(); c != children.end(); ++c)
            (*c)->sizefun(width, height);
    }
};

#endif