#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#undef STB_IMAGE_IMPLEMENTATION

#include <nonstd.h>

#include "renderer.h"

void *backgroundThread(void *args);

int main()
{
    program_state_t current_state = INIT;
    set_current_state(&current_state);


    // arraylist_test();
    // hashmap_test();
    // tripplebuffer_tests();


    tripplebuffer_t window_test_tripplebuffer;
    memset(&window_test_tripplebuffer, 0, sizeof(tripplebuffer_t));
    tripplebuffer_alloc(&window_test_tripplebuffer, 1, sizeof(double));

    pthread_t backThread;
    pthread_create(&backThread, NULL, backgroundThread, &window_test_tripplebuffer);

    pthread_t frontThread;
    pthread_create(&frontThread, NULL, foregroundThread, &window_test_tripplebuffer);

    pthread_join(backThread, NULL);
    pthread_join(frontThread, NULL);

    tripplebuffer_free(&window_test_tripplebuffer);

    return 0;
}

void *backgroundThread(void *args)
{
    struct timespec ts;
    tripplebuffer_t *tripplebuffer = (tripplebuffer_t *)args;
    program_state_t current_state = INVALID_STATE;
    get_current_state(&current_state);

    double color_rate = 1.0 / 5.0;
    double dt = 1.0 / 1000.0;

    double *prev_value = NULL;
    double *new_value = NULL;
    tripplebuffer_get_ptr((void **)&prev_value, tripplebuffer, 0, BACK);
    *prev_value = 0.0;
    tripplebuffer_swap_back(tripplebuffer);

    while (current_state != STOP)
    {
        if (current_state == RUN)
        {
            tripplebuffer_get_ptr((void **)&new_value, tripplebuffer, 0, BACK);
            *new_value = *prev_value + (color_rate * dt);
            while (*new_value > 1.0)
            {
                *new_value -= 1.0;
            }
            prev_value = new_value;
            tripplebuffer_swap_back(tripplebuffer);
        }
        get_current_state(&current_state);
        ts.tv_sec = 0l;
        ts.tv_nsec = (long)(dt * 1.0E9);
        while (nanosleep(&ts, &ts))
            ;
    }
    pthread_exit(NULL);
}


