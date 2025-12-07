#include "reference.h"
#include "shared.h"
#include <math.h>
#include <time.h>

#define DT_REF 0.12
#define DT_MODEL 0.05
#define PI 3.14159265358979

void* thread_reference_gen(void* arg) {
    struct timespec next;
    clock_gettime(CLOCK_MONOTONIC, &next);
    while (running) {
        struct timespec t0, t1;
        clock_gettime(CLOCK_MONOTONIC, &t0);

        SharedData d = get_shared_data();
        double t = d.t;
        d.xref = 5.0 / PI * cos(0.2 * PI * t);
        d.yref = (t < 10.0 ? 1 : -1) * 5.0 / PI * sin(0.2 * PI * t);
        set_shared_data(d);

        clock_gettime(CLOCK_MONOTONIC, &t1);
        double dt = (t1.tv_sec - t0.tv_sec)*1e3 + (t1.tv_nsec - t0.tv_nsec)/1e6;
        if (exec_index[3] < 1000) exec_times[3][exec_index[3]++] = dt;

        next.tv_nsec += (long)(DT_REF * 1e9);
        if (next.tv_nsec >= 1e9) { next.tv_sec++; next.tv_nsec -= 1e9; }
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
    }
    return NULL;
}

void* thread_ref_model_x(void* arg) {
    struct timespec next;
    clock_gettime(CLOCK_MONOTONIC, &next);
    while (running) {
        struct timespec t0, t1;
        clock_gettime(CLOCK_MONOTONIC, &t0);

        SharedData d = get_shared_data();
        d.ymx += d.alpha1 * (d.xref - d.ymx) * DT_MODEL;
        set_shared_data(d);

        clock_gettime(CLOCK_MONOTONIC, &t1);
        double dt = (t1.tv_sec - t0.tv_sec)*1e3 + (t1.tv_nsec - t0.tv_nsec)/1e6;
        if (exec_index[4] < 1000) exec_times[4][exec_index[4]++] = dt;

        next.tv_nsec += (long)(DT_MODEL * 1e9);
        if (next.tv_nsec >= 1e9) { next.tv_sec++; next.tv_nsec -= 1e9; }
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
    }
    return NULL;
}

void* thread_ref_model_y(void* arg) {
    struct timespec next;
    clock_gettime(CLOCK_MONOTONIC, &next);
    while (running) {
        struct timespec t0, t1;
        clock_gettime(CLOCK_MONOTONIC, &t0);

        SharedData d = get_shared_data();
        d.ymy += d.alpha2 * (d.yref - d.ymy) * DT_MODEL;
        set_shared_data(d);

        clock_gettime(CLOCK_MONOTONIC, &t1);
        double dt = (t1.tv_sec - t0.tv_sec)*1e3 + (t1.tv_nsec - t0.tv_nsec)/1e6;
        if (exec_index[5] < 1000) exec_times[5][exec_index[5]++] = dt;

        next.tv_nsec += (long)(DT_MODEL * 1e9);
        if (next.tv_nsec >= 1e9) { next.tv_sec++; next.tv_nsec -= 1e9; }
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
    }
    return NULL;
}
