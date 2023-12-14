#ifndef GLFW_ENABLED_H
#define GLFW_ENABLED_H
#include <algorithm>
#include <vector>
#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string>

enum eGLFWEvent
{
    CHAR,
    CHARMOD,
    CURSORENTER,
    CURSORPOS,
    DROP,
    FRAMEBUFFERSIZE,
    JOYSTICK,
    KEY,
    MONITOR,
    MOUSEBUTTON,
    SCROLL,
    CLOSE,
    CONTENTSCALE,
    FOCUS,
    ICONIFY,
    MAXIMIZE,
    POS,
    REFRESH,
    SIZE
};

struct CHAR_EVENT
{
    unsigned int codepoint;
};
struct CHARMOD_EVENT
{
    unsigned int codepoint;
    int mods;
};
struct CURSORENTER_EVENT
{
    int entered;
};
struct CURSORPOS_EVENT
{
    double xpos;
    double ypos;
};
struct DROP_EVENT
{
    int path_count;
    const char **paths;
};
struct FRAMEBUFFERSIZE_EVENT
{
    int width;
    int height;
};
struct JOYSTICK_EVENT
{
    int jid;
    int event;
};
struct KEY_EVENT
{
    int key;
    int scancode;
    int action;
    int mods;
};
struct MONITOR_EVENT
{
    GLFWmonitor *monitor;
    int event;
};
struct MOUSEBUTTON_EVENT
{
    int button;
    int action;
    int mods;
};
struct SCROLL_EVENT
{
    double xoffset;
    double yoffset;
};
struct CLOSE_EVENT
{
};
struct CONTENTSCALE_EVENT
{
    float xscale;
    float yscale;
};
struct FOCUS_EVENT
{
    int focused;
};
struct ICONIFY_EVENT
{
    int iconified;
};
struct MAXIMIZE_EVENT
{
    int maximized;
};
struct POS_EVENT
{
    int xpos;
    int ypos;
};
struct REFRESH_EVENT
{
};
struct SIZE_EVENT
{
    int width;
    int height;
};

struct GLFW_EVENT
{
    eGLFWEvent Type;
    union
    {
        CHAR_EVENT mChar;
        CHARMOD_EVENT mCharMod;
        CURSORENTER_EVENT mCursorEnter;
        CURSORPOS_EVENT mCursorPos;
        DROP_EVENT mDrop;
        FRAMEBUFFERSIZE_EVENT mFrameBufferSize;
        JOYSTICK_EVENT mJoystick;
        KEY_EVENT mKey;
        MONITOR_EVENT mMonitor;
        MOUSEBUTTON_EVENT mMouseButton;
        SCROLL_EVENT mScroll;
        CLOSE_EVENT mClose;
        CONTENTSCALE_EVENT mContenteScale;
        FOCUS_EVENT mFocus;
        ICONIFY_EVENT mIconify;
        MAXIMIZE_EVENT mMaximize;
        POS_EVENT mPos;
        REFRESH_EVENT mRefresh;
        SIZE_EVENT mSize;
    };
};

class glfw_enabled
{
private:
    std::vector<glfw_enabled *> children;

public:
    virtual void adopt(glfw_enabled *child);
    virtual glfw_enabled *orphan(glfw_enabled *child);
    virtual void cleanup();
    virtual void update(float dt);
    virtual void draw();
    virtual bool handleEvent(GLFW_EVENT event);
};

#endif