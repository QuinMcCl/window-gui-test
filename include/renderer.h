#ifndef RENDERER_H
#define RENDERER_H

#ifdef __cplusplus
extern "C"
{
#endif

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

    typedef struct render_args_s render_args_t;
    struct render_args_s
    {
        task_queue_t *ptr_tq;
    };

    void RenderThread(void *args);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif