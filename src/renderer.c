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

int inputs[GLFW_KEY_LAST + 1];

typedef enum movement_directions_e
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    NUM_MOVEMENT_DIRECTIONS
} movement_directions_t;

int imputMovementMap[NUM_MOVEMENT_DIRECTIONS] = {
    GLFW_KEY_W,
    GLFW_KEY_S,
    GLFW_KEY_A,
    GLFW_KEY_D,
    GLFW_KEY_SPACE,
    GLFW_KEY_LEFT_CONTROL,
};

vec3 vecMovementMap[NUM_MOVEMENT_DIRECTIONS] = {
    ((vec3){0.0f, 0.0f, -1.0f}),
    ((vec3){0.0f, 0.0f, 1.0f}),
    ((vec3){-1.0f, 0.0f, 0.0f}),
    ((vec3){1.0f, 0.0f, 0.0f}),
    ((vec3){0.0f, 1.0f, 0.0f}),
    ((vec3){0.0f, -1.0f, 0.0f}),
};

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

void update_camera_pos(float dt, int inputs[GLFW_KEY_LAST + 1]);

void RenderThread(void *args)
{
    THROW_ERR(args == NULL, "NULL RENDER ARGS", return);
    render_args_t *r_args = args;

    struct timespec last_time;
    struct timespec this_time;
    shader_t shader;
    vec3 camera_pos = {0.0f, 0.5f, 5.0f};
    vec3 world_up = {0.0f, 1.0f, 0.0f};
    int numModels = 1;
    model_t models[1];

    // INIT
    {
        CHECK(get_current_state(&current_state), return);
        memset(inputs, GLFW_RELEASE, sizeof(inputs));

        // tripplebuffer_t *tripplebuffer = (tripplebuffer_t *)args;

        CHECK(window_init(&main_window, 1920, 1080, "MainWindow", 0.3f, 0.3f, 0.3f, 1.0f), return);
        old_GLFWwindowclosecallback = glfwSetWindowCloseCallback(main_window.window, windowclosefun);
        old_framebuffersizecallback = glfwSetFramebufferSizeCallback(main_window.window, framebuffersizefun);
        old_keycallback = glfwSetKeyCallback(main_window.window, keyfun);
        old_cursorposcallback = glfwSetCursorPosCallback(main_window.window, cursorposfun);

        if (glfwRawMouseMotionSupported())
            glfwSetInputMode(main_window.window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

        CHECK(texture_unit_freelist_alloc(), return);
        CHECK(loaded_textures_alloc(MAX_LOADED_TEXTURE_COUNT), return);
        CHECK(nonstd_opengl_ubo_bindingpoints_alloc(), return);

        // start imgui
        CHECK(imgui_init(&gui, main_window.window), return);

        memset(&shader, 0, sizeof(shader_t));
        CHECK(shader_init(&shader, "./shaders/instanced_flat.vs", "./shaders/instanced_flat.fs"), return);

        memset(&camera, 0, sizeof(camera_t));
        CHECK(camera_alloc(&camera, camera_pos, world_up, -90.0f, 0.0f, 0.0f, 0.1f, 100.0f, main_window.aspect, 45.0f), return);
        CHECK(shader_bindBuffer(&shader, camera.mViewProjection.name, camera.mViewProjection.bindingPoint), return);
        CHECK(shader_bindBuffer(&shader, camera.mViewPosition.name, camera.mViewPosition.bindingPoint), return);

        memset(models, 0, sizeof(models));

        // CHECK(model_alloc(&(models[0]), &shader, "./resources/objects/Bunny/", "bun_zipper.ply", 14), return);
        CHECK(model_alloc(&(models[0]), &shader, "./resources/objects/vampire/", "dancing_vampire.dae", 20), return);

        current_state = RUN;
        CHECK(set_current_state(current_state), return);
    }

    mat4 transform = GLM_MAT4_IDENTITY_INIT;

    // RUN
    clock_gettime(CLOCK_MONOTONIC, &last_time);
    while (current_state != STOP && !gui.options.file_options.should_close)
    {
        glfwPollEvents();

        clock_gettime(CLOCK_MONOTONIC, &this_time);
        double deltaTime = (double)(this_time.tv_sec - last_time.tv_sec) + ((double)(this_time.tv_nsec - last_time.tv_nsec) * 1.0E-9);
        last_time = this_time;

        glm_rotate(transform, deltaTime * (1.0 / 5.0), vecMovementMap[UP]);

        update_camera_pos(deltaTime, inputs);

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
            CHECK(model_draw(&models[i], transform), return);
        }

        // guidraw
        CHECK(imgui_draw(&gui, r_args->ptr_tq, 1, &camera, numModels, models), return);

        // swap
        window_swap(&main_window);

        CHECK(get_current_state(&current_state), return);
    }

    current_state = STOP;
    CHECK(set_current_state(current_state), return);

    window_cleanup(&main_window);

    CHECK(camera_free(&camera), return);
    for (int i = 0; i < numModels; i++)
    {
        CHECK(model_free(&(models[i])), return);
    }
    CHECK(shader_free(&shader), return);

    CHECK(nonstd_opengl_ubo_bindingpoints_free(), return);
    CHECK(loaded_textures_free(), return);
    CHECK(texture_unit_freelist_free(), return);

    glfwTerminate();
    return;
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
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
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
        if (!gui.paused && action != GLFW_REPEAT)
        {
            inputs[key] = action;
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

void update_camera_pos(float dt, int inputs[GLFW_KEY_LAST + 1])
{
    vec3 move;
    mat3 view;
    for (int dir = 0; dir < NUM_MOVEMENT_DIRECTIONS; dir++)
    {
        if (inputs[imputMovementMap[dir]] == GLFW_PRESS)
        {
            glm_vec3_copy(vecMovementMap[dir], move);
            glm_vec3_scale(move, dt, move);
            glm_mat4_pick3(camera.mView, view);
            glm_mat3_inv(view, view);
            glm_mat3_mulv(view, move, move);
            glm_vec3_add(move, camera.mPosition, camera.mPosition);
        }
    }
}