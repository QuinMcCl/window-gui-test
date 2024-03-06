#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

#include <nonstd.h>
#include <nonstd_glfw_opengl.h>
#include <nonstd_imgui.h>
#include <tile_map.h>

#include "renderer.h"

#define MAX_LOADED_TEXTURE_COUNT 800

// nonstd_glfw_window_t tmp_window;
window_t main_window = {0};
nonstd_imgui_t gui = {0};
camera_t camera = {0};
program_state_t current_state = INVALID_STATE;
tile_t *loaded_tiles[2] = {0};
map_t the_map = {0};

int inputs[GLFW_KEY_LAST + 1] = {GLFW_RELEASE};

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
GLFWdropfun old_dropcallback;

void windowclosefun(GLFWwindow *window);
void framebuffersizefun(GLFWwindow *window, int width, int height);
void keyfun(GLFWwindow *window, int key, int scancode, int action, int mods);
void cursorposfun(GLFWwindow *window, double xpos, double ypos);
void dropfun(GLFWwindow *window, int count, const char **paths);

void update_camera_pos(float dt, int inputs[GLFW_KEY_LAST + 1]);

void RenderThread(void *args)
{
    CHECK_ERR(args == NULL ? EINVAL : EXIT_SUCCESS, strerror(errno), return);
    render_args_t *r_args = args;

    // struct timespec last_time = {0};
    // struct timespec this_time = {0};
    // shader_t shader = {0};
    shader_t map_shader = {0};
    // vec3 camera_pos = {0.0f, 0.5f, 5.0f};
    // vec3 world_up = {0.0f, 1.0f, 0.0f};
    // int numModels = 1;
    // model_t models[1] = {0};

    // mat4 transform = GLM_MAT4_IDENTITY_INIT;

    // INIT
    {
        // tripplebuffer_t *tripplebuffer = (tripplebuffer_t *)args;

        CHECK_ERR(window_init(&main_window, 1920, 1080, "MainWindow", 0.3f, 0.3f, 0.3f, 1.0f), strerror(errno), return);
        old_GLFWwindowclosecallback = glfwSetWindowCloseCallback(main_window.window, windowclosefun);
        old_framebuffersizecallback = glfwSetFramebufferSizeCallback(main_window.window, framebuffersizefun);
        old_keycallback = glfwSetKeyCallback(main_window.window, keyfun);
        old_cursorposcallback = glfwSetCursorPosCallback(main_window.window, cursorposfun);
        old_dropcallback = glfwSetDropCallback(main_window.window, dropfun);

        if (glfwRawMouseMotionSupported())
            glfwSetInputMode(main_window.window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

        CHECK_ERR(texture_unit_freelist_alloc(), strerror(errno), return);
        CHECK_ERR(nonstd_opengl_ubo_bindingpoints_alloc(), strerror(errno), return);
        CHECK_ERR(loaded_textures_alloc(MAX_LOADED_TEXTURE_COUNT), strerror(errno), return);

        // start imgui
        CHECK_ERR(imgui_init(&gui, main_window.window), strerror(errno), return);

        // CHECK_ERR(shader_init(&shader, "./shaders/instanced_flat.vs", "./shaders/instanced_flat.fs"), strerror(errno), return);
        CHECK_ERR(shader_init(&map_shader, "./shaders/tile.vs", "./shaders/tile.fs"), strerror(errno), return);

        // CHECK_ERR(camera_alloc(&camera, camera_pos, world_up, -90.0f, 0.0f, 0.0f, 0.1f, 100.0f, main_window.aspect, 45.0f), strerror(errno), return);
        // CHECK_ERR(shader_bindBuffer(&shader, camera.mViewProjection.name, camera.mViewProjection.bindingPoint), strerror(errno), return);
        // CHECK_ERR(shader_bindBuffer(&shader, camera.mViewPosition.name, camera.mViewPosition.bindingPoint), strerror(errno), return);

        // CHECK_ERR(model_alloc(&(models[0]), &shader, "./resources/objects/Bunny/", "bun_zipper.ply", 14), strerror(errno), return);
        // CHECK_ERR(model_alloc(&(models[0]), &shader, "./resources/objects/vampire/", "dancing_vampire.dae", 20), strerror(errno), return);

        init_map(&the_map, r_args->ptr_tq, sizeof(loaded_tiles), loaded_tiles, &map_shader, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

        current_state = RUN;
        CHECK_ERR(set_current_state(current_state), strerror(errno), return);
    }

    // RUN
    // clock_gettime(CLOCK_MONOTONIC, &last_time);
    while (current_state != STOP && !gui.options.file_options.should_close)
    {
        glfwPollEvents();
        MAP_POP_LOADED(the_map);
        // clock_gettime(CLOCK_MONOTONIC, &this_time);
        // double deltaTime = (double)(this_time.tv_sec - last_time.tv_sec) + ((double)(this_time.tv_nsec - last_time.tv_nsec) * 1.0E-9);
        // last_time = this_time;

        // glm_rotate(transform, deltaTime * (1.0 / 5.0), vecMovementMap[UP]);

        // update_camera_pos(deltaTime, inputs);

        // camera_update_view_projection(&camera);

        window_clear(&main_window);

        // draw
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);

        MAP_DRAW(the_map);

        // for (int i = 0; i < numModels; i++)
        // {
        //     CHECK_ERR(model_draw(&models[i], transform), strerror(errno), return);
        // }

        // guidraw
        // CHECK_ERR(imgui_draw(&gui, r_args->ptr_tq, 1, &camera, numModels, models), strerror(errno), return);
        CHECK_ERR(imgui_draw(&gui, r_args->ptr_tq, 0, NULL, 0, NULL), strerror(errno), return);

        // swap
        window_swap(&main_window);

        CHECK_ERR(get_current_state(&current_state), strerror(errno), return);
    }

    current_state = STOP;
    CHECK_ERR(set_current_state(current_state), strerror(errno), return);

    window_cleanup(&main_window);

    CHECK_ERR(camera_free(&camera), strerror(errno), return);
    // for (int i = 0; i < numModels; i++)
    // {
    //     CHECK_ERR(model_free(&(models[i])), strerror(errno), return);
    // }
    // CHECK_ERR(shader_free(&shader), strerror(errno), return);
    CHECK_ERR(shader_free(&map_shader), strerror(errno), return);

    CHECK_ERR(nonstd_opengl_ubo_bindingpoints_free(), strerror(errno), return);
    CHECK_ERR(loaded_textures_free(), strerror(errno), return);
    CHECK_ERR(texture_unit_freelist_free(), strerror(errno), return);

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

void dropfun(GLFWwindow *window, int count, const char **paths)
{
#ifdef CONTEXT_SWITCHING
    GLFWwindow *oldContext = glfwGetCurrentContext();
    glfwMakeContextCurrent(main_window.window);
#endif
    assert(window != NULL);
    if (count > 0)
    {
        fprintf(stderr, "%s", paths[0]);
        MAP_RELOAD(the_map, paths[0]);
    }
    if (old_dropcallback != NULL)
    {
        old_dropcallback(window, count, paths);
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
