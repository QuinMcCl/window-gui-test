#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#undef STB_IMAGE_IMPLEMENTATION

#include <nonstd.h>

#include "renderer.h"
typedef struct engine_args_s engine_args_t;
struct engine_args_s
{
    task_queue_t * ptr_tq;
};
// void *backgroundThread(void *args);

int main()
{
    // arraylist_test();
    // hashmap_test();
    // tripplebuffer_tests();
    // async_test();

#define MAX_CONCURRENT_TASKS 1024UL
#define MAX_THREADS 8UL

    async_task_t task_buffer[MAX_CONCURRENT_TASKS] = {0};
    pthread_t threads[MAX_THREADS] = {0};
    task_queue_t tq = {0};
    task_queue_init(&tq, sizeof(task_buffer), task_buffer, NULL, NULL, sizeof(threads), threads, NULL, NULL, NULL);

    // tripplebuffer_t window_test_tripplebuffer = {0};
    // tripplebuffer_alloc(&window_test_tripplebuffer, 1, sizeof(double));

    async_task_t task = {0};
    render_args_t render_args = {.ptr_tq = &tq};
    engine_args_t engine_args = {.ptr_tq = &tq};

    program_state_t current_state = INIT;
    CHECK(set_current_state(current_state), return retval);


    // task.func = backgroundThread;
    // task.funcName = "backgroundThread";
    // task.args = &(engine_args);

    // while (tq.queue.push(&(tq.queue), sizeof(task), &task))
    // {
    //     fprintf(stdout, "task queue full\n");
    // }
    // tq.awake(&tq);

    task.func = RenderThread;
    task.funcName = "RenderThread";
    task.args = &(render_args);

    while (tq.queue.push(&(tq.queue), sizeof(task), &task))
    {
        fprintf(stdout, "task queue full\n");
    }
    tq.awake(&tq);


    CHECK(wait_until_state(STOP), return retval);
    // tripplebuffer_free(&window_test_tripplebuffer);

    // TODO kill task queue
    return 0;
}

// void *backgroundThread(void *args)
// {
//     struct timespec ts;
//     tripplebuffer_t *tripplebuffer = (tripplebuffer_t *)args;
//     program_state_t current_state = INVALID_STATE;
//     get_current_state(&current_state);

//     double color_rate = 1.0 / 5.0;
//     double dt = 1.0 / 1000.0;

//     double *prev_value = NULL;
//     double *new_value = NULL;
//     tripplebuffer_get_ptr((void **)&prev_value, tripplebuffer, 0, BACK);
//     *prev_value = 0.0;
//     tripplebuffer_swap_back(tripplebuffer);

//     while (current_state != STOP)
//     {
//         if (current_state == RUN)
//         {
//             tripplebuffer_get_ptr((void **)&new_value, tripplebuffer, 0, BACK);
//             *new_value = *prev_value + (color_rate * dt);
//             while (*new_value > 1.0)
//             {
//                 *new_value -= 1.0;
//             }
//             prev_value = new_value;
//             tripplebuffer_swap_back(tripplebuffer);
//         }
//         get_current_state(&current_state);
//         ts.tv_sec = 0l;
//         ts.tv_nsec = (long)(dt * 1.0E9);
//         while (nanosleep(&ts, &ts))
//             ;
//     }
//     pthread_exit(NULL);
// }
