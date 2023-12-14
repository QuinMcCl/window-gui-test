#ifndef GLFW_ENABLED_H
#define GLFW_ENABLED_H
#include <algorithm>
#include <vector>
#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>

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

    virtual void cleanup()
    {
        for (std::vector<glfw_enabled *>::iterator c = this->children.begin(); c != this->children.end(); ++c)
            (*c)->cleanup();
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

    virtual bool handleEvent(GLFW_EVENT event)
    {
        bool stop = false;
        for (std::vector<glfw_enabled *>::iterator c = this->children.begin(); !stop && c != this->children.end(); ++c)
            stop |= (*c)->handleEvent(event);
        return stop;
    };
};

class uniformBufferedObject
{
private:
    static unsigned int mAllBindingPoints;
    unsigned int mBindingPoint = GL_FALSE;
    unsigned int uboBlock = GL_FALSE;
    unsigned int mMaxSize = GL_FALSE;
    unsigned int mUsage;
    std::string mName = " ";

public:
    uniformBufferedObject(const std::string name, const unsigned int maxSize, unsigned int usage);

    void fill(const void *items, unsigned int size, unsigned int offset);

    void cleanup();

    std::string getName();

    unsigned int getBlockBindingIndex();
};

#endif