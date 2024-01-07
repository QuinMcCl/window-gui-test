#include <stdio.h>
#include <math.h>
#include <time.h>

#include <nonstd.h>
#include <nonstd_glfw_opengl.h>
#include <nonstd_imgui.h>

#include "renderer.h"

#define MAX_LOADED_TEXTURE_COUNT 800

// nonstd_glfw_window_t tmp_window;
window_t main_window;
nonstd_imgui_t gui;
program_state_t current_state = INVALID_STATE;


GLFWwindowclosefun old_GLFWwindowclosecallback;
GLFWframebuffersizefun old_framebuffersizecallback;
void windowclosefun(GLFWwindow *window);
void framebuffersizefun(GLFWwindow *window, int width, int height);

void *RenderThread(void *args)
{
    CHECK(get_current_state(&current_state), pthread_exit(args));

    // struct timespec lastframe;
    // struct timespec currentFrame;
    // tripplebuffer_t *tripplebuffer = (tripplebuffer_t *)args;

    CHECK(window_init(&main_window, 1920, 1080, "MainWindow", 0.3f, 0.3f, 0.3f, 1.0f), pthread_exit(args));
    old_GLFWwindowclosecallback = glfwSetWindowCloseCallback(main_window.window, windowclosefun);
    old_framebuffersizecallback = glfwSetFramebufferSizeCallback(main_window.window, framebuffersizefun);

    CHECK(texture_unit_freelist_alloc(), pthread_exit(args));
    CHECK(loaded_textures_alloc(MAX_LOADED_TEXTURE_COUNT),  pthread_exit(args));
    CHECK(nonstd_opengl_ubo_bindingpoints_alloc(), pthread_exit(args));

    // start imgui
    CHECK(imgui_init(&gui, main_window.window), pthread_exit(args));



    


    current_state = RUN;
    CHECK(set_current_state(&current_state), pthread_exit(args));
    // clock_gettime(CLOCK_MONOTONIC, &lastframe);

    while (current_state != STOP && !gui.options.file_options.should_close)
    {
        // clock_gettime(CLOCK_MONOTONIC, &currentFrame);
        // double deltaTime = (double)(currentFrame.tv_sec - lastframe.tv_sec) + ((double)(currentFrame.tv_nsec - lastframe.tv_nsec) * 1.0E9);
        // lastframe = currentFrame;

        window_clear(&main_window);

        // draw

        // guidraw
        imgui_draw(&gui);

        // swap
        window_swap(&main_window);

        CHECK(get_current_state(&current_state), pthread_exit(args));
    }

    current_state = STOP;
    CHECK(set_current_state(&current_state), pthread_exit(args));

    window_cleanup(&main_window);

    
    CHECK(nonstd_opengl_ubo_bindingpoints_free(), pthread_exit(args));
    CHECK(loaded_textures_free(), pthread_exit(args));
    CHECK(texture_unit_freelist_free(), pthread_exit(args));



    glfwTerminate();
    pthread_exit(args);
}



void windowclosefun(GLFWwindow *window)
{
#ifdef CONTEXT_SWITCHING
    GLFWwindow *oldContext = glfwGetCurrentContext();
    glfwMakeContextCurrent(main_window.window);
#endif
    gui.options.file_options.requesting_close = glfwWindowShouldClose(window);
    if (old_GLFWwindowclosecallback != NULL)
    {
        old_GLFWwindowclosecallback(window);
    }
#ifdef CONTEXT_SWITCHING
    glfwMakeContextCurrent(oldContext);
#endif
}

void framebuffersizefun(GLFWwindow *window, int width, int height)
{
#ifdef CONTEXT_SWITCHING
    GLFWwindow *oldContext = glfwGetCurrentContext();
    glfwMakeContextCurrent(main_window.window);
#endif
    glViewport(0, 0, width, height);
    glfwGetFramebufferSize(main_window.window, &(main_window.width), &(main_window.height));
    main_window.aspect = (float)main_window.width / (float)main_window.height;

    if (old_framebuffersizecallback != NULL)
    {
        old_framebuffersizecallback(window, width, height);
    }
#ifdef CONTEXT_SWITCHING
    glfwMakeContextCurrent(oldContext);
#endif
}