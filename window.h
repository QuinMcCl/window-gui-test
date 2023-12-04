#ifndef WINDOW_H
#define WINDOW_H
#include "types.h"
#include "glfw_enabled.h"


class Window : public glfw_enabled
{
private:
    GLFWwindow *mWindow = NULL;
    int window_initialized = GL_FALSE;
    unsigned int mSCR_WIDTH;
    unsigned int mSCR_HEIGHT;


public:
    Window(int x, int y);
    ~Window();
    int init_window();
    void draw();
    int ShouldClose();
    void framebuffersizefun(int width, int height);
    void keyfun(int key, int scancode, int action, int mods);


private:
    bool is_current();
    void make_current();





};

#endif