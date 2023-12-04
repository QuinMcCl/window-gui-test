#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include "window.h"
#include "shader.h"

double lastTime = 0.0;
double deltaTime = 0.0;
double prev_xpos = 0.0;
double prev_ypos = 0.0;

Window main_window(1920, 1080);

Shader sprite_shader("../sprite.vs", "../sprite.fs");

void GLFW_ErrorCallback(int i , const char *err_str);

int main()
{
    // Register error callback first
    glfwSetErrorCallback(GLFW_ErrorCallback);
    // Start GLFW next
    const int ret = glfwInit();
    if (GL_TRUE != ret)
    {
        /* Problem: glfwInit failed, something is seriously wrong. */
        fprintf(stderr, "GLFW Error!\n");
        return -1;
    }

    if (main_window.init_window() != GL_TRUE)
    {
        return -1;
    }

    // glew: load all OpenGL function pointers
    // ---------------------------------------
    // If using GLEW version 1.13 or earlier
    glewExperimental = GL_TRUE;
    GLenum glew_err = glewInit();
    if (GLEW_OK != glew_err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_err));
        return -1;
    }

    if (sprite_shader.read_and_compile() != GL_TRUE)
    {
        return -1;
    }

    // render loop
    // -----------
    while (!main_window.ShouldClose())
    {
        glfwPollEvents();
        double currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        // fprintf(stderr, "FPS: %f\n", 1.0 / deltaTime);
        lastTime = glfwGetTime();

        main_window.update(deltaTime);

        main_window.draw();
    }

    main_window.~Window();
    sprite_shader.~Shader();

    return 0;
}


void GLFW_ErrorCallback(int i , const char *err_str)
{
    fprintf(stderr, "Error: %s\n", err_str);
}
