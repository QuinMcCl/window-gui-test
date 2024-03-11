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
#define NUM_TILES 16 * 16
// static nonstd_glfw_window_t tmp_window;
static window_t main_window = {0};
static nonstd_imgui_t gui = {0};
static camera_t camera = {0};
static program_state_t current_state = INVALID_STATE;
static tile_t *loaded_tiles[NUM_TILES] = {0};
static tile_t map_tiles[16 * 16] = {0};
static map_t the_map = {0};
// static int map_tile_index = 0;

static int inputs[GLFW_KEY_LAST + 1] = {GLFW_RELEASE};

static int imputMovementMap[NUM_MOVEMENT_DIRECTIONS] = {
    GLFW_KEY_W,
    GLFW_KEY_S,
    GLFW_KEY_A,
    GLFW_KEY_D,
    GLFW_KEY_SPACE,
    GLFW_KEY_LEFT_CONTROL,
};

static vec3 vecMovementMap[NUM_MOVEMENT_DIRECTIONS] = {
    ((vec3){0.0f, 0.0f, -1.0f}),
    ((vec3){0.0f, 0.0f, 1.0f}),
    ((vec3){-1.0f, 0.0f, 0.0f}),
    ((vec3){1.0f, 0.0f, 0.0f}),
    ((vec3){0.0f, 1.0f, 0.0f}),
    ((vec3){0.0f, -1.0f, 0.0f}),
};

static double lastX = 0.0;
static double lastY = 0.0;

static GLFWwindowclosefun old_GLFWwindowclosecallback;
static GLFWframebuffersizefun old_framebuffersizecallback;
static GLFWkeyfun old_keycallback;
static GLFWcursorposfun old_cursorposcallback;
// static GLFWdropfun old_dropcallback;

static void windowclosefun(GLFWwindow *window);
static void framebuffersizefun(GLFWwindow *window, int width, int height);
static void keyfun(GLFWwindow *window, int key, int scancode, int action, int mods);
static void cursorposfun(GLFWwindow *window, double xpos, double ypos);
// static void dropfun(GLFWwindow *window, int count, const char **paths);

static void update_camera_pos(float dt, int inputs[GLFW_KEY_LAST + 1]);

void RenderThread(void *args)
{
    CHECK_ERR(args == NULL ? EINVAL : EXIT_SUCCESS, strerror(errno), return);
    render_args_t *r_args = args;

    struct timespec this_time = {0};
    struct timespec last_time = {0};
    shader_t camera_shader = {0};
    shader_t map_shader = {0};
    vec3 camera_pos = {0.0f, 0.5f, 5.0f};
    vec3 world_up = {0.0f, 1.0f, 0.0f};
    int numModels = 1;
    model_t models[1] = {0};

    mat4 transform = GLM_MAT4_IDENTITY_INIT;

    int retval = 0;
    double stop_work = 0.0;
    double time_worked = 0.0;
    double deltaTime = 0.0;

    // INIT
    {
        // tripplebuffer_t *tripplebuffer = (tripplebuffer_t *)args;

        CHECK_ERR(window_init(&main_window, 1920, 1080, "MainWindow", 0.3f, 0.3f, 0.3f, 1.0f), strerror(errno), return);
        old_GLFWwindowclosecallback = glfwSetWindowCloseCallback(main_window.window, windowclosefun);
        old_framebuffersizecallback = glfwSetFramebufferSizeCallback(main_window.window, framebuffersizefun);
        old_keycallback = glfwSetKeyCallback(main_window.window, keyfun);
        old_cursorposcallback = glfwSetCursorPosCallback(main_window.window, cursorposfun);
        // old_dropcallback = glfwSetDropCallback(main_window.window, dropfun);

        if (glfwRawMouseMotionSupported())
            glfwSetInputMode(main_window.window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

        CHECK_ERR(texture_unit_freelist_alloc(), strerror(errno), return);
        CHECK_ERR(nonstd_opengl_ubo_bindingpoints_alloc(), strerror(errno), return);
        CHECK_ERR(loaded_textures_alloc(MAX_LOADED_TEXTURE_COUNT), strerror(errno), return);

        // start imgui
        CHECK_ERR(imgui_init(&gui, main_window.window), strerror(errno), return);

        CHECK_ERR(shader_init(&camera_shader, "./shaders/instanced_flat.vs", "./shaders/instanced_flat.fs"), strerror(errno), return);
        CHECK_ERR(shader_init(&map_shader, "./shaders/tile.vs", "./shaders/tile.fs"), strerror(errno), return);

        CHECK_ERR(camera_alloc(&camera, camera_pos, world_up, -90.0f, 0.0f, 0.0f, 0.1f, 100.0f, main_window.aspect, 45.0f), strerror(errno), return);
        CHECK_ERR(shader_bindBuffer(&camera_shader, camera.mViewProjection.name, camera.mViewProjection.bindingPoint), strerror(errno), return);
        CHECK_ERR(shader_bindBuffer(&camera_shader, camera.mViewPosition.name, camera.mViewPosition.bindingPoint), strerror(errno), return);

        // CHECK_ERR(model_alloc(&(models[0]), &camera_shader, "./resources/objects/Bunny/", "bun_zipper.ply", 14), strerror(errno), return);
        CHECK_ERR(model_alloc(&(models[0]), &camera_shader, "./resources/objects/vampire/", "dancing_vampire.dae", 20), strerror(errno), return);

        // TODO FIX?
        CHECK_ERR(init_map(&the_map, "/home/conductor/usgs/landsatTiles", NUM_TILES, map_tiles, r_args->ptr_tq, sizeof(loaded_tiles), loaded_tiles, &map_shader, NULL, NULL, NULL, NULL, NULL, NULL, NULL), strerror(errno), return);

        current_state = RUN;
        CHECK_ERR(set_current_state(current_state), strerror(errno), return);
    }

    // RUN
    clock_gettime(CLOCK_MONOTONIC, &last_time);
    while (current_state != STOP && !gui.options.file_options.should_close)
    {
        clock_gettime(CLOCK_MONOTONIC, &this_time);
        deltaTime = (double)(this_time.tv_sec - last_time.tv_sec) + ((double)(this_time.tv_nsec - last_time.tv_nsec) * 1.0E-9);
        last_time = this_time;
        // fprintf(stdout, "%.6f\n", 1.0 / deltaTime);
        stop_work = (double)(this_time.tv_sec) + ((double)(this_time.tv_nsec) * 1.0E-9) + (0.8 / 60.0);
        time_worked = 0.0;
        glfwPollEvents();

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
            CHECK_ERR(model_draw(&models[i], transform), strerror(errno), return);
        }

        glDepthFunc(GL_LEQUAL);
        MAP_DRAW(the_map);

        glDepthFunc(GL_LESS);
        // gui draw
        CHECK_ERR(imgui_draw(&gui, r_args->ptr_tq, 1, &camera, numModels, models), strerror(errno), return);
        // CHECK_ERR(imgui_draw(&gui, r_args->ptr_tq, 0, NULL, 0, NULL), strerror(errno), return);

        do
        {
            retval = MAP_POP_LOADED(the_map);
            clock_gettime(CLOCK_MONOTONIC, &this_time);
            time_worked = (double)(this_time.tv_sec) + ((double)(this_time.tv_nsec) * 1.0E-9);
        } while (!retval && time_worked < stop_work);

        // swap
        window_swap(&main_window);
        CHECK_ERR(get_current_state(&current_state), strerror(errno), return);
    }

    current_state = STOP;
    CHECK_ERR(set_current_state(current_state), strerror(errno), return);

    window_cleanup(&main_window);

    CHECK_ERR(camera_free(&camera), strerror(errno), return);
    for (int i = 0; i < numModels; i++)
    {
        CHECK_ERR(model_free(&(models[i])), strerror(errno), return);
    }
    CHECK_ERR(shader_free(&camera_shader), strerror(errno), return);
    CHECK_ERR(shader_free(&map_shader), strerror(errno), return);

    CHECK_ERR(loaded_textures_free(), strerror(errno), return);
    CHECK_ERR(nonstd_opengl_ubo_bindingpoints_free(), strerror(errno), return);
    CHECK_ERR(texture_unit_freelist_free(), strerror(errno), return);

    glfwTerminate();
    return;
}

static void windowclosefun(GLFWwindow *window)
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

static void framebuffersizefun(GLFWwindow *window, int width, int height)
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

static void keyfun(GLFWwindow *window, int key, int scancode, int action, int mods)
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

static void cursorposfun(GLFWwindow *window, double xpos, double ypos)
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

// static void dropfun(GLFWwindow *window, int count, const char **paths)
// {
// #ifdef CONTEXT_SWITCHING
//     GLFWwindow *oldContext = glfwGetCurrentContext();
//     glfwMakeContextCurrent(main_window.window);
// #endif
//     assert(window != NULL);
//     for (int i = 0; i < count; i++)
//     {
//         fprintf(stderr, "%s\n", paths[i]);
//         MAP_RELOAD(the_map, &(the_map.tile_array[map_tile_index]), paths[i]);
//         map_tile_index++;
//         map_tile_index %= NUM_TILES;
//     }
//     if (old_dropcallback != NULL)
//     {
//         old_dropcallback(window, count, paths);
//     }
// #ifdef CONTEXT_SWITCHING
//     glfwMakeContextCurrent(oldContext);
// #endif
// }

static void update_camera_pos(float dt, int inputs[GLFW_KEY_LAST + 1])
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
