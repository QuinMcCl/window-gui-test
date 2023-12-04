#include "window.h"
#include <stdio.h>

void window_charfun(GLFWwindow *window, unsigned int codepoint)
{
    Window *this_window = static_cast<Window *>(glfwGetWindowUserPointer(window));
    if (this_window == NULL)
        return;
    this_window->charfun(codepoint);
}
void window_charmodsfun(GLFWwindow *window, unsigned int codepoint, int mods)
{
    Window *this_window = static_cast<Window *>(glfwGetWindowUserPointer(window));
    if (this_window == NULL)
        return;
    this_window->charmodsfun(codepoint, mods);
}
void window_cursorenterfun(GLFWwindow *window, int entered)
{
    Window *this_window = static_cast<Window *>(glfwGetWindowUserPointer(window));
    if (this_window == NULL)
        return;
    this_window->cursorenterfun(entered);
}
void window_cursorposfun(GLFWwindow *window, double xpos, double ypos)
{
    Window *this_window = static_cast<Window *>(glfwGetWindowUserPointer(window));
    if (this_window == NULL)
        return;
    this_window->cursorposfun(xpos, ypos);
}
void window_dropfun(GLFWwindow *window, int path_count, const char **paths)
{
    Window *this_window = static_cast<Window *>(glfwGetWindowUserPointer(window));
    if (this_window == NULL)
        return;
    this_window->dropfun(path_count, paths);
}
void window_framebuffersizefun(GLFWwindow *window, int width, int height)
{
    Window *this_window = static_cast<Window *>(glfwGetWindowUserPointer(window));
    if (this_window == NULL)
        return;
    this_window->framebuffersizefun(width, height);
}
void window_joystickfun(int jid, int event)
{
    GLFWwindow *window = glfwGetCurrentContext();
    if (window == NULL)
        return;
    Window *this_window = static_cast<Window *>(glfwGetWindowUserPointer(window));
    if (this_window == NULL)
        return;
    this_window->joystickfun(jid, event);
}
void window_keyfun(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    Window *this_window = static_cast<Window *>(glfwGetWindowUserPointer(window));
    if (this_window == NULL)
        return;
    this_window->keyfun(key, scancode, action, mods);
}
void window_monitorfun(GLFWmonitor *monitor, int event)
{
    GLFWwindow *window = glfwGetCurrentContext();
    if (window == NULL)
        return;
    Window *this_window = static_cast<Window *>(glfwGetWindowUserPointer(window));
    if (this_window == NULL)
        return;
    this_window->monitorfun(monitor, event);
}
void window_mousebuttonfun(GLFWwindow *window, int button, int action, int mods)
{
    Window *this_window = static_cast<Window *>(glfwGetWindowUserPointer(window));
    if (this_window == NULL)
        return;
    this_window->mousebuttonfun(button, action, mods);
}
void window_scrollfun(GLFWwindow *window, double xoffset, double yoffset)
{
    Window *this_window = static_cast<Window *>(glfwGetWindowUserPointer(window));
    if (this_window == NULL)
        return;
    this_window->scrollfun(xoffset, yoffset);
}
void window_closefun(GLFWwindow *window)
{
    Window *this_window = static_cast<Window *>(glfwGetWindowUserPointer(window));
    if (this_window == NULL)
        return;
    this_window->closefun();
}
void window_contentscalefun(GLFWwindow *window, float xscale, float yscale)
{
    Window *this_window = static_cast<Window *>(glfwGetWindowUserPointer(window));
    if (this_window == NULL)
        return;
    this_window->contentscalefun(xscale, yscale);
}
void window_focusfun(GLFWwindow *window, int focused)
{
    Window *this_window = static_cast<Window *>(glfwGetWindowUserPointer(window));
    if (this_window == NULL)
        return;
    this_window->focusfun(focused);
}
void window_iconifyfun(GLFWwindow *window, int iconified)
{
    Window *this_window = static_cast<Window *>(glfwGetWindowUserPointer(window));
    if (this_window == NULL)
        return;
    this_window->iconifyfun(iconified);
}
void window_maximizefun(GLFWwindow *window, int maximized)
{
    Window *this_window = static_cast<Window *>(glfwGetWindowUserPointer(window));
    if (this_window == NULL)
        return;
    this_window->maximizefun(maximized);
}
void window_posfun(GLFWwindow *window, int xpos, int ypos)
{
    Window *this_window = static_cast<Window *>(glfwGetWindowUserPointer(window));
    if (this_window == NULL)
        return;
    this_window->posfun(xpos, ypos);
}
void window_refreshfun(GLFWwindow *window)
{
    Window *this_window = static_cast<Window *>(glfwGetWindowUserPointer(window));
    if (this_window == NULL)
        return;
    this_window->refreshfun();
}
void window_sizefun(GLFWwindow *window, int width, int height)
{
    Window *this_window = static_cast<Window *>(glfwGetWindowUserPointer(window));
    if (this_window == NULL)
        return;
    this_window->sizefun(width, height);
}

Window::Window(int x = 800, int y = 600) : glfw_enabled()
{
    mSCR_WIDTH = x;
    mSCR_HEIGHT = y;

}

Window::~Window()
{

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
}

int Window::init_window()
{
    if (window_initialized == GL_TRUE)
        return window_initialized;

    // glfw: initialize and configure
    // ------------------------------
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    mWindow = glfwCreateWindow(mSCR_WIDTH, mSCR_HEIGHT, "window_gui_tests", NULL, NULL);
    if (mWindow == NULL)
    {
        fprintf(stderr, "Failed to create GLFW window");
        return window_initialized;
    }

    glfwMakeContextCurrent(mWindow);
    glfwSetWindowUserPointer(mWindow, this);

    glfwSetCharCallback(mWindow, window_charfun);
    glfwSetCharModsCallback(mWindow, window_charmodsfun);
    glfwSetCharModsCallback(mWindow, window_charmodsfun);
    glfwSetCursorEnterCallback(mWindow, window_cursorenterfun);
    glfwSetCursorPosCallback(mWindow, window_cursorposfun);
    glfwSetDropCallback(mWindow, window_dropfun);
    glfwSetFramebufferSizeCallback(mWindow, window_framebuffersizefun);
    glfwSetJoystickCallback(window_joystickfun);
    glfwSetKeyCallback(mWindow, window_keyfun);
    glfwSetMonitorCallback(window_monitorfun);
    glfwSetMouseButtonCallback(mWindow, window_mousebuttonfun);
    glfwSetScrollCallback(mWindow, window_scrollfun);
    glfwSetWindowCloseCallback(mWindow, window_closefun);
    glfwSetWindowContentScaleCallback(mWindow, window_contentscalefun);
    glfwSetWindowFocusCallback(mWindow, window_focusfun);
    glfwSetWindowIconifyCallback(mWindow, window_iconifyfun);
    glfwSetWindowMaximizeCallback(mWindow, window_maximizefun);
    glfwSetWindowPosCallback(mWindow, window_posfun);
    glfwSetWindowRefreshCallback(mWindow, window_refreshfun);
    glfwSetWindowSizeCallback(mWindow, window_sizefun);

    glfwSetInputMode(mWindow, GLFW_STICKY_KEYS, GL_TRUE);
    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(mWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    window_initialized = GL_TRUE;
    return window_initialized;
}

void Window::draw()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (!is_current())
        make_current();

    glfw_enabled::draw();
    
    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(mWindow);
}

int Window::ShouldClose()
{
    if (!is_current())
        make_current();
    return glfwWindowShouldClose(mWindow);
}

bool Window::is_current()
{
    return glfwGetCurrentContext() == mWindow;
}

void Window::make_current()
{
    glfwMakeContextCurrent(mWindow);
}

void Window::framebuffersizefun(int width, int height)
{
    if (!is_current())
        make_current();
    mSCR_WIDTH = width;
    mSCR_HEIGHT = height;
    glViewport(0, 0, mSCR_WIDTH, mSCR_HEIGHT);
}


void Window::keyfun(int key, int scancode, int action, int mods) 
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(mWindow, true);
    glfw_enabled::keyfun(key, scancode, action, mods);
}