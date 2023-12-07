#ifndef WINDOW_H
#define WINDOW_H

#include "glfw_enabled.h"

class Window : public glfw_enabled
{
public:
    Window(unsigned int width, unsigned int height, const char *title);
    ~Window();

    bool shouldClose();
    void keyfun(int key, int scancode, int action, int mods) override;

    void draw() override;
    GLFWwindow *mWindow;

private:
    void checkContext();
    static bool has_glfw_initialized;
    static bool has_glew_initialized;
    unsigned int mWidth;
    unsigned int mHeight;
};

#endif