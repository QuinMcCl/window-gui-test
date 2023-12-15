#include <iostream>

#include "glfw_enabled.h"
#include "window.h"

bool Window::has_glfw_initialized = GL_FALSE;
bool Window::has_glew_initialized = GL_FALSE;

void glfw_errcallback(int error_code, const char *description)
{
   /* Problem: glfw failed, something is seriously wrong. */
   fprintf(stderr, "Error%d:%s\n", error_code, description);
}

void windowCallback(GLFWwindow *window, GLFW_EVENT event)
{
   Window *some_window = static_cast<Window *>(glfwGetWindowUserPointer(window));
   if (some_window != NULL)
      some_window->handleEvent(event);
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
   GLFWwindow *thisWindow;
   thisWindow = glfwCreateWindow(mWidth, mHeight, title, NULL, NULL);
   if (thisWindow == NULL)
   {
      fprintf(stderr, "Failed to create GLFW window\n");
   }
   glfwMakeContextCurrent(thisWindow);
   glfwSetWindowUserPointer(thisWindow, this);
   //  glfwSetInputMode(thisWindow, GLFW_STICKY_KEYS, GLFW_TRUE);

   glfwSetCharCallback(thisWindow, [](GLFWwindow *window, unsigned int codepoint) -> void
                       { windowCallback(window, (GLFW_EVENT){.Type = CHAR, .mChar = {.codepoint = codepoint}}); });

   glfwSetCharModsCallback(thisWindow, [](GLFWwindow *window, unsigned int codepoint, int mods) -> void
                           { windowCallback(window, (GLFW_EVENT){.Type = CHARMOD, .mCharMod = {.codepoint = codepoint, .mods = mods}}); });

   glfwSetCursorEnterCallback(thisWindow, [](GLFWwindow *window, int entered) -> void
                              { windowCallback(window, (GLFW_EVENT){.Type = CURSORENTER, .mCursorEnter = {.entered = entered}}); });

   glfwSetCursorPosCallback(thisWindow, [](GLFWwindow *window, double xpos, double ypos) -> void
                            { windowCallback(window, (GLFW_EVENT){.Type = CURSORPOS, .mCursorPos = {.xpos = xpos, .ypos = ypos}}); });

   glfwSetDropCallback(thisWindow, [](GLFWwindow *window, int path_count, const char **paths) -> void
                       { windowCallback(window, (GLFW_EVENT){.Type = DROP, .mDrop = {.path_count = path_count, .paths = paths}}); });

   glfwSetFramebufferSizeCallback(thisWindow, [](GLFWwindow *window, int width, int height) -> void
                                  { windowCallback(window, (GLFW_EVENT){.Type = FRAMEBUFFERSIZE, .mFrameBufferSize = {.width = width, .height = height}}); });

   // glfwSetJoystickCallback([thisWindow](int jid, int event) -> void
   //                         { windowCallback(thisWindow, (GLFW_EVENT){.Type = JOYSTICK, .mJoystick = {.jid = jid, .event = event}}); });

   glfwSetKeyCallback(thisWindow, [](GLFWwindow *window, int key, int scancode, int action, int mods) -> void
                      { windowCallback(window, (GLFW_EVENT){.Type = KEY, .mKey = {key, scancode, action, mods}}); });

   // glfwSetMonitorCallback([thisWindow](GLFWmonitor *monitor, int event) -> void
   //                        { windowCallback(thisWindow, (GLFW_EVENT){.Type = MONITOR, .mMonitor = {.monitor= monitor,.event= event}}); });

   glfwSetMouseButtonCallback(thisWindow, [](GLFWwindow *window, int button, int action, int mods) -> void
                              { windowCallback(window, (GLFW_EVENT){.Type = MOUSEBUTTON, .mMouseButton = {.button= button,.action= action,.mods= mods}}); });

   glfwSetScrollCallback(thisWindow, [](GLFWwindow *window, double xoffset, double yoffset) -> void
                         { windowCallback(window, (GLFW_EVENT){.Type = SCROLL, .mScroll = {.xoffset= xoffset,.yoffset= yoffset}}); });

   glfwSetWindowCloseCallback(thisWindow, [](GLFWwindow *window) -> void
                              { windowCallback(window, (GLFW_EVENT){.Type = CLOSE, .mClose = {}}); });

   glfwSetWindowContentScaleCallback(thisWindow, [](GLFWwindow *window, float xscale, float yscale) -> void
                                     { windowCallback(window, (GLFW_EVENT){.Type = CONTENTSCALE, .mContenteScale = {.xscale= xscale,.yscale= yscale}}); });

   glfwSetWindowFocusCallback(thisWindow, [](GLFWwindow *window, int focused) -> void
                              { windowCallback(window, (GLFW_EVENT){.Type = FOCUS, .mFocus = {.focused= focused}}); });

   glfwSetWindowIconifyCallback(thisWindow, [](GLFWwindow *window, int iconified) -> void
                                { windowCallback(window, (GLFW_EVENT){.Type = ICONIFY, .mIconify = {.iconified= iconified}}); });

   glfwSetWindowMaximizeCallback(thisWindow, [](GLFWwindow *window, int maximized) -> void
                                 { windowCallback(window, (GLFW_EVENT){.Type = MAXIMIZE, .mMaximize = {.maximized= maximized}}); });

   glfwSetWindowPosCallback(thisWindow, [](GLFWwindow *window, int xpos, int ypos) -> void
                            { windowCallback(window, (GLFW_EVENT){.Type = POS, .mPos = {.xpos= xpos,.ypos= ypos}}); });

   glfwSetWindowRefreshCallback(thisWindow, [](GLFWwindow *window) -> void
                                { windowCallback(window, (GLFW_EVENT){.Type = REFRESH, .mRefresh = {}}); });

   glfwSetWindowSizeCallback(thisWindow, [](GLFWwindow *window, int width, int height) -> void
                             { windowCallback(window, (GLFW_EVENT){.Type = SIZE, .mSize = {.width= width,.height= height}}); });

   mWindow = thisWindow;

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

void Window::cleanup()
{
   checkContext();
   glfwSetWindowShouldClose(mWindow, GL_TRUE);
   glfwDestroyWindow(mWindow);
   glfw_enabled::cleanup();
}

bool Window::shouldClose()
{
   checkContext();
   return glfwWindowShouldClose(mWindow);
}

int Window::getWidth()
{
   return mWidth;
}

int Window::getHeight()
{
   return mHeight;
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

bool Window::handleEvent(GLFW_EVENT event)
{
   bool stop = false;
   switch (event.Type)
   {
   case FRAMEBUFFERSIZE:
      stop |= framebuffersizefun(event.mFrameBufferSize);
      break;
   case KEY:
      stop |= keyfun(event.mKey);
      break;
   default:
      break;
   }

   if (!stop)
   stop |= glfw_enabled::handleEvent(event);

   return stop;
}

bool Window::framebuffersizefun(FRAMEBUFFERSIZE_EVENT event)
{
   bool stop = false;
   mWidth = event.width;
   mHeight = event.height;
   glViewport(0, 0, mWidth, mHeight);
   stop = true;
   return stop;
}

bool Window::keyfun(KEY_EVENT event)
{
   bool stop = false;
   if (event.key == GLFW_KEY_ESCAPE && event.action == GLFW_PRESS)
   {
      stop = true;
      glfwSetWindowShouldClose(mWindow, GLFW_TRUE);
   }
   return stop;
}

void Window::checkContext()
{
   if (glfwGetCurrentContext() != mWindow)
   {
      glfwMakeContextCurrent(mWindow);
   }
}
