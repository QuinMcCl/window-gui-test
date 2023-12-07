#include <iostream>

#include "window.h"

bool Window::has_glfw_initialized = GL_FALSE;
bool Window::has_glew_initialized = GL_FALSE;

void glfw_errcallback(int error_code, const char *description)
{
    /* Problem: glfw failed, something is seriously wrong. */
    fprintf(stderr, "Error%d:%s\n", error_code, description);
}

void windowcharfun(GLFWwindow *window, unsigned int codepoint)
{
   Window * some_window = static_cast<Window *>( glfwGetWindowUserPointer(window));
   some_window->charfun(codepoint);
}
void windowcharmodsfun(GLFWwindow *window, unsigned int codepoint, int mods)
{
   Window * some_window = static_cast<Window *>( glfwGetWindowUserPointer(window));
   some_window->charmodsfun(codepoint, mods);
}
void windowcursorenterfun(GLFWwindow *window, int entered)
{
   Window * some_window = static_cast<Window *>( glfwGetWindowUserPointer(window));
   some_window->cursorenterfun(entered);
}
void windowcursorposfun(GLFWwindow *window, double xpos, double ypos)
{
   Window * some_window = static_cast<Window *>( glfwGetWindowUserPointer(window));
   some_window->cursorposfun(xpos, ypos);
}
void windowdropfun(GLFWwindow *window, int path_count, const char **paths)
{
   Window * some_window = static_cast<Window *>( glfwGetWindowUserPointer(window));
   some_window->dropfun(path_count, paths);
}
void windowframebuffersizefun(GLFWwindow *window, int width, int height)
{
   Window * some_window = static_cast<Window *>( glfwGetWindowUserPointer(window));
   some_window->framebuffersizefun(width, height);
}
void windowjoystickfun(int jid, int event)
{
//    Window * some_window = static_cast<Window *>( glfwGetWindowUserPointer(window));
//    some_window->joystickfun(jid, event);
}
void windowkeyfun(GLFWwindow *window, int key, int scancode, int action, int mods)
{
   Window * some_window = static_cast<Window *>( glfwGetWindowUserPointer(window));
   some_window->keyfun(key, scancode, action, mods);
}
void windowmonitorfun(GLFWmonitor *monitor, int event)
{
//    Window * some_window = static_cast<Window *>( glfwGetWindowUserPointer(window));
//    some_window->monitorfun(monitor, event);
}
void windowmousebuttonfun(GLFWwindow *window, int button, int action, int mods)
{
   Window * some_window = static_cast<Window *>( glfwGetWindowUserPointer(window));
   some_window->mousebuttonfun(button, action, mods);
}
void windowscrollfun(GLFWwindow *window, double xoffset, double yoffset)
{
   Window * some_window = static_cast<Window *>( glfwGetWindowUserPointer(window));
   some_window->scrollfun(xoffset, yoffset);
}
void windowclosefun(GLFWwindow *window)
{
   Window * some_window = static_cast<Window *>( glfwGetWindowUserPointer(window));
   some_window->closefun();
}
void windowcontentscalefun(GLFWwindow *window, float xscale, float yscale)
{
   Window * some_window = static_cast<Window *>( glfwGetWindowUserPointer(window));
   some_window->contentscalefun(xscale, yscale);
}
void windowfocusfun(GLFWwindow *window, int focused)
{
   Window * some_window = static_cast<Window *>( glfwGetWindowUserPointer(window));
   some_window->focusfun(focused);
}
void windowiconifyfun(GLFWwindow *window, int iconified)
{
   Window * some_window = static_cast<Window *>( glfwGetWindowUserPointer(window));
   some_window->iconifyfun(iconified);
}
void windowmaximizefun(GLFWwindow *window, int maximized)
{
   Window * some_window = static_cast<Window *>( glfwGetWindowUserPointer(window));
   some_window->maximizefun(maximized);
}
void windowposfun(GLFWwindow *window, int xpos, int ypos)
{
   Window * some_window = static_cast<Window *>( glfwGetWindowUserPointer(window));
   some_window->posfun(xpos, ypos);
}
void windowrefreshfun(GLFWwindow *window)
{
   Window * some_window = static_cast<Window *>( glfwGetWindowUserPointer(window));
   some_window->refreshfun();
}
void windowsizefun(GLFWwindow *window, int width, int height)
{
   Window * some_window = static_cast<Window *>( glfwGetWindowUserPointer(window));
   some_window->sizefun(width, height);
}

Window::Window(unsigned int width, unsigned int height, const char *title)
{
    if (has_glfw_initialized != GLFW_TRUE)
    {
        glfwSetErrorCallback(glfw_errcallback);
        // glfw: initialize and configure
        // ------------------------------
        has_glfw_initialized = glfwInit();
        if (GLFW_TRUE != has_glfw_initialized)
        {
            fprintf(stderr, "Error WITH GLFW\n");
            return;
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    }

    mWidth = width;
    mHeight = height;
    // glfw window creation
    // --------------------
    mWindow = glfwCreateWindow(mWidth, mHeight, title, NULL, NULL);
    if (mWindow == NULL)
    {
        fprintf(stderr, "Failed to create GLFW window\n");
    }
    glfwMakeContextCurrent(mWindow);
    glfwSetWindowUserPointer(mWindow, this);

    glfwSetCharCallback(mWindow, windowcharfun);
    glfwSetCharModsCallback(mWindow, windowcharmodsfun);
    glfwSetCursorEnterCallback(mWindow, windowcursorenterfun);
    glfwSetCursorPosCallback(mWindow, windowcursorposfun);
    glfwSetDropCallback(mWindow, windowdropfun);
    glfwSetFramebufferSizeCallback(mWindow, windowframebuffersizefun);
    glfwSetJoystickCallback(windowjoystickfun);
    glfwSetKeyCallback(mWindow, windowkeyfun);
    glfwSetMonitorCallback(windowmonitorfun);
    glfwSetMouseButtonCallback(mWindow, windowmousebuttonfun);
    glfwSetScrollCallback(mWindow, windowscrollfun);
    glfwSetWindowCloseCallback(mWindow, windowclosefun);
    glfwSetWindowContentScaleCallback(mWindow, windowcontentscalefun);
    glfwSetWindowFocusCallback(mWindow, windowfocusfun);
    glfwSetWindowIconifyCallback(mWindow, windowiconifyfun);
    glfwSetWindowMaximizeCallback(mWindow, windowmaximizefun);
    glfwSetWindowPosCallback(mWindow, windowposfun);
    glfwSetWindowRefreshCallback(mWindow, windowrefreshfun);
    glfwSetWindowSizeCallback(mWindow, windowsizefun);

    if (has_glew_initialized != GL_TRUE)
    {

        // glew: load all OpenGL function pointers
        // ---------------------------------------
        // If using GLEW version 1.13 or earlier
        glewExperimental = GL_TRUE;
        GLenum glew_err = glewInit();
        if (GLEW_OK != glew_err)
        {
            /* Problem: glewInit failed, something is seriously wrong. */
            fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_err));
            return;
        }
        has_glew_initialized = GL_TRUE;
    }
}

Window::~Window()
{
    checkContext();
    glfwSetWindowShouldClose(mWindow, GL_TRUE);
    glfwDestroyWindow(mWindow);
}

bool Window::shouldClose()
{
    checkContext();
    return glfwWindowShouldClose(mWindow);
}

void Window::draw()
{
    checkContext();
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfw_enabled::draw();
    glfwSwapBuffers(mWindow);
    glfwPollEvents();
}

void Window::checkContext()
{
    if (glfwGetCurrentContext() != mWindow)
    {
        glfwMakeContextCurrent(mWindow);
    }
}