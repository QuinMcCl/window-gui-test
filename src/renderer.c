#include <stdio.h>
#include <math.h>
#include <time.h>

#include <nonstd.h>
#include <nonstd_glfw_opengl.h>
#include <nonstd_imgui.h>

#include "renderer.h"

void *foregroundThread(void *args)
{
    program_state_t current_state = INVALID_STATE;
    get_current_state(&current_state);

    nonstd_glfw_window_t tmp_window;
    nonstd_glfw_window_t main_window;
    nonstd_imgui_t gui;

    struct timespec lastframe;
    struct timespec currentFrame;
    tripplebuffer_t *tripplebuffer = (tripplebuffer_t *)args;

    // Our state

    double h = 0.0;
    double c = 1.0;
    double x = 0.0;
    double m = 0.0;

    float red, green, blue;

    nonstd_glfw_window_init(&tmp_window, 100, 100, "tmp_window", 0.0f, 0.0f, 0.0f, 1.0f);
    nonstd_glfw_window_cleanup(&tmp_window);
    nonstd_glfw_window_init(&main_window, 1920, 1080, "MainWindow", 0.0f, 0.0f, 0.0f, 1.0f);

    glfwSetWindowUserPointer(main_window.window, &main_window);
    install_callbacks(main_window.window);

    // start imgui
    nonstd_imgui_init(&gui, main_window.window);
    nonstd_glfw_adopt((nonstd_glfw_t *)&main_window, (nonstd_glfw_t *)&gui);

    current_state = RUN;
    set_current_state(&current_state);

    clock_gettime(CLOCK_MONOTONIC, &lastframe);

    while (current_state != STOP && !gui.options.file_options.should_close)
    {

        clock_gettime(CLOCK_MONOTONIC, &currentFrame);
        double deltaTime = (double)(currentFrame.tv_sec - lastframe.tv_sec) + ((double)(currentFrame.tv_nsec - lastframe.tv_nsec) * 1.0E9);
        lastframe = currentFrame;

        glfwPollEvents();
        tripplebuffer_swap_front(tripplebuffer);
        tripplebuffer_cpy_out_front((void *)&h, tripplebuffer, 0, 1);

        // color update
        {
            x = 1.0 - fabs(fmod(6.0 * h, 2.0) - 1);

            if (h < (1.0 / 6.0))
            {
                red = m + c;
                green = m + x;
                blue = m;
            }
            else if (h < (2.0 / 6.0))
            {
                red = m + x;
                green = m + c;
                blue = m;
            }
            else if (h < (3.0 / 6.0))
            {
                red = m;
                green = m + c;
                blue = m + x;
            }
            else if (h < (4.0 / 6.0))
            {
                red = m;
                green = m + x;
                blue = m + c;
            }
            else if (h < (5.0 / 6.0))
            {
                red = m + x;
                green = m;
                blue = m + c;
            }
            else
            {
                red = m + c;
                green = m;
                blue = m + x;
            }
            nonstd_glfw_window_set_clear_color(&main_window, red, green, blue, 1.0f);
        }

        main_window.base.update(&main_window, deltaTime);

        // draw
        main_window.base.draw(&main_window);
        // swap
        nonstd_glfw_window_swap(&main_window);

        get_current_state(&current_state);
    }

    current_state = STOP;
    set_current_state(&current_state);

    nonstd_glfw_window_cleanup(&main_window);
    glfwTerminate();

    pthread_exit(NULL);
}