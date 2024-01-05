#include <stdio.h>
#include <math.h>

#include <nonstd.h>
#include <nonstd_glfw_opengl.h>
#include <nonstd_imgui.h>

#include "renderer.h"

void *foregroundThread(void *args)
{
    program_state_t current_state = INVALID_STATE;
    get_current_state(&current_state);

    nonstd_glfw_t imgui_handle_blocker;
    nonstd_glfw_window_t tmp_window;
    nonstd_glfw_window_t main_window;

    struct timespec ts;
    tripplebuffer_t *tripplebuffer = (tripplebuffer_t *)args;

    // Our state
    double dt = 1.0 / 60.0;

    imgui_main_menu_options_t main_menu_options;

    double h = 0.0;
    double c = 1.0;
    double x = 0.0;
    double m = 0.0;

    float red, green, blue;

    // init imgui_handle_blocker
    {
        imgui_handle_blocker.update = nonstd_glfw_update;
        imgui_handle_blocker.draw = nonstd_glfw_draw;
        imgui_handle_blocker.cleanup = nonstd_glfw_cleanup;
        imgui_handle_blocker.event_handler = nonstd_imgui_event_blocker;
        imgui_handle_blocker.sibling = NULL;
        imgui_handle_blocker.child = NULL;
    }

    nonstd_glfw_window_init(&tmp_window, 100, 100, "tmp_window", 0.0f, 0.0f, 0.0f, 1.0f);
    nonstd_glfw_window_cleanup(&tmp_window);
    nonstd_glfw_window_init(&main_window, 1920, 1080, "MainWindow", 0.0f, 0.0f, 0.0f, 1.0f);

    nonstd_glfw_adopt(&imgui_handle_blocker, (nonstd_glfw_t *)&main_window);

    glfwSetWindowUserPointer(main_window.window, &imgui_handle_blocker);
    install_callbacks(main_window.window);

    // start imgui
    nonstd_imgui_init(main_window.window);

    current_state = RUN;
    set_current_state(&current_state);

    while (current_state != STOP)
    {
        glfwPollEvents();
        tripplebuffer_swap_front(tripplebuffer);
        tripplebuffer_cpy_out_front((void *)&h, tripplebuffer, 0, 1);
        nonstd_glfw_window_get_should_close(&main_window, (int *)&(main_menu_options.file_options.should_close));

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
        }

        // new imgui frame
        nonstd_imgui_start_frame();

        // my draw calls
        {
            nonstd_glfw_window_set_clear_color(&main_window, red, green, blue, 1.0f);
            nonstd_glfw_window_draw(&main_window);
        }

        // imgui draw calls
        {
            if (main_menu_options.file_options.should_close)
            {
                ShowClosePopUp(&(main_menu_options.file_options), &current_state);
                nonstd_glfw_window_set_should_close(&main_window, (int *)&(main_menu_options.file_options.should_close));
                set_current_state(&current_state);
            }
            ShowMainMenu(&main_menu_options);
        }

        // end imgui frame
        nonstd_imgui_end_frame();

        // swap
        nonstd_glfw_window_swap(&main_window);

        get_current_state(&current_state);

        ts.tv_sec = 0l;
        ts.tv_nsec = (long)(dt * 1.0E9);
        while (nanosleep(&ts, &ts))
            ;
    }

    nonstd_imgui_cleanup();

    nonstd_glfw_window_cleanup(&main_window);
    glfwTerminate();

    pthread_exit(NULL);
}