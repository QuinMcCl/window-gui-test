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
camera_t camera;
program_state_t current_state = INVALID_STATE;

double lastX = 0.0;
double lastY = 0.0;

GLFWwindowclosefun old_GLFWwindowclosecallback;
GLFWframebuffersizefun old_framebuffersizecallback;
GLFWkeyfun old_keycallback;
GLFWcursorposfun old_cursorposcallback;

void windowclosefun(GLFWwindow *window);
void framebuffersizefun(GLFWwindow *window, int width, int height);
void keyfun(GLFWwindow *window, int key, int scancode, int action, int mods);
void cursorposfun(GLFWwindow *window, double xpos, double ypos);

void *RenderThread(void *args)
{
    CHECK(get_current_state(&current_state), pthread_exit(args));

    struct timespec start_time;
    struct timespec end_time;
    // tripplebuffer_t *tripplebuffer = (tripplebuffer_t *)args;

    CHECK(window_init(&main_window, 1920, 1080, "MainWindow", 0.3f, 0.3f, 0.3f, 1.0f), pthread_exit(args));
    old_GLFWwindowclosecallback = glfwSetWindowCloseCallback(main_window.window, windowclosefun);
    old_framebuffersizecallback = glfwSetFramebufferSizeCallback(main_window.window, framebuffersizefun);
    old_keycallback = glfwSetKeyCallback(main_window.window, keyfun);
    old_cursorposcallback = glfwSetCursorPosCallback(main_window.window, cursorposfun);

    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(main_window.window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    CHECK(texture_unit_freelist_alloc(), pthread_exit(args));
    CHECK(loaded_textures_alloc(MAX_LOADED_TEXTURE_COUNT), pthread_exit(args));
    CHECK(nonstd_opengl_ubo_bindingpoints_alloc(), pthread_exit(args));

    // start imgui
    CHECK(imgui_init(&gui, main_window.window), pthread_exit(args));

    shader_t shader;
    memset(&shader, 0, sizeof(shader_t));
    CHECK(shader_init(&shader, "./shaders/instanced_flat.vs", "./shaders/instanced_flat.fs"), pthread_exit(args));

    memset(&camera, 0, sizeof(camera_t));
    vec3 camera_pos = {0.0f, 0.5f, 5.0f};
    vec3 world_up = {0.0f, 1.0f, 0.0f};
    CHECK(camera_alloc(&camera, camera_pos, world_up, -90.0f, 0.0f, 0.0f, 0.1f, 100.0f, main_window.aspect, 45.0f), pthread_exit(args));

    int numModels = 1;
    model_t models[1];
    memset(models, 0, sizeof(models));

    CHECK(model_alloc(&(models[0]), &shader, "./resources/objects/vampire/", "dancing_vampire.dae", 20), pthread_exit(args));
    // CHECK(model_alloc(&(models[1]), &shader, "./resources/objects/rock/", "rock.obj", 20), pthread_exit(args));

    current_state = RUN;
    CHECK(set_current_state(&current_state), pthread_exit(args));

    while (current_state != STOP && !gui.options.file_options.should_close)
    {
        clock_gettime(CLOCK_MONOTONIC, &start_time);
        glfwPollEvents();

        camera_update_view_projection(&camera);

        window_clear(&main_window);

        // draw
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);

        for (int i = 0; i < numModels; i++)
        {
            CHECK(model_draw(&models[i]), pthread_exit(args));
        }
        clock_gettime(CLOCK_MONOTONIC, &end_time);

        // guidraw
        CHECK(imgui_draw(&gui, 1, &camera, numModels, models), pthread_exit(args));

        // swap
        window_swap(&main_window);

        // double deltaTime = (double)(end_time.tv_sec - start_time.tv_sec) + ((double)(end_time.tv_nsec - start_time.tv_nsec) * 1.0E-9);
        // fprintf(stderr, "Start: %lu %lu\t End: %lu %lu\n", start_time.tv_sec, start_time.tv_nsec, end_time.tv_sec, end_time.tv_nsec);
        // fprintf(stderr, "DT: %.3f\tFPS: %.3f\n", deltaTime, 1.0 / deltaTime);

        CHECK(get_current_state(&current_state), pthread_exit(args));
    }

    current_state = STOP;
    CHECK(set_current_state(&current_state), pthread_exit(args));

    window_cleanup(&main_window);

    CHECK(camera_free(&camera), pthread_exit(args));
    for (int i = 0; i < numModels; i++)
    {
        CHECK(model_free(&(models[i])), pthread_exit(args));
    }
    CHECK(shader_free(&shader), pthread_exit(args));

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
    gui.paused = 1;
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

void keyfun(GLFWwindow *window, int key, int scancode, int action, int mods)
{
#ifdef CONTEXT_SWITCHING
    GLFWwindow *oldContext = glfwGetCurrentContext();
    glfwMakeContextCurrent(main_window.window);
#endif
    if (!imgui_capture_key())
    {
        switch (key)
        {
        case GLFW_KEY_ESCAPE:
            if (action == GLFW_PRESS)
            {
                gui.paused = !gui.paused;
                if (gui.paused)
                {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                }
                else
                {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    glfwGetCursorPos(window, &lastX, &lastY);
                }
            }
            break;
        default:
            break;
        }
    }
    if (old_keycallback != NULL)
    {
        old_keycallback(window, key, scancode, action, mods);
    }
#ifdef CONTEXT_SWITCHING
    glfwMakeContextCurrent(oldContext);
#endif
}

void cursorposfun(GLFWwindow *window, double xpos, double ypos)
{
#ifdef CONTEXT_SWITCHING
    GLFWwindow *oldContext = glfwGetCurrentContext();
    glfwMakeContextCurrent(main_window.window);
#endif
    if (!gui.paused)
    {
        // fprintf(stderr, "%f, %f\n", xpos, ypos);
        float xoffset = xpos - lastX;
        float yoffset = -ypos + lastY; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        xoffset *= camera.mMouseSensitivity;
        yoffset *= camera.mMouseSensitivity;

        // fprintf(stderr, "X:%.5f, Y:%.5f\n", xoffset, yoffset);
        camera.mYaw += xoffset;
        camera.mPitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (camera.mConstrainPitch)
        {
            if (camera.mPitch > 89.0f)
                camera.mPitch = 89.0f;
            if (camera.mPitch < -89.0f)
                camera.mPitch = -89.0f;
        }
        // glfwSetCursorPos(window, 0, 0);
    }

    if (old_cursorposcallback != NULL)
    {
        old_cursorposcallback(window, xpos, ypos);
    }
#ifdef CONTEXT_SWITCHING
    glfwMakeContextCurrent(oldContext);
#endif
}
