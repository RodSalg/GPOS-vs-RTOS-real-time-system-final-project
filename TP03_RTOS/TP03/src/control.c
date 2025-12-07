#include "control.h"
#include "shared.h"
#include <math.h>
#include <time.h>

#define DT_LINEAR 0.04
#define DT_CONTROL 0.05
#define R 1.0 

void* thread_linearization(void* arg) {
    struct timespec next;
    clock_gettime(CLOCK_MONOTONIC, &next);
    while (running) {
        struct timespec t0, t1;
        clock_gettime(CLOCK_MONOTONIC, &t0);

        SharedData d = get_shared_data();
        double theta = d.x[2];
        double u1 = d.v[0]*cos(theta) + d.v[1]*sin(theta);
        double u2 = (-d.v[0]*sin(theta) + d.v[1]*cos(theta)) / R;
        d.u[0] = u1;
        d.u[1] = u2;
        set_shared_data(d);

        clock_gettime(CLOCK_MONOTONIC, &t1);
        double dt = (t1.tv_sec - t0.tv_sec)*1e3 + (t1.tv_nsec - t0.tv_nsec)/1e6;
        if (exec_index[1] < 1000) exec_times[1][exec_index[1]++] = dt;

        next.tv_nsec += (long)(DT_LINEAR * 1e9);
        if (next.tv_nsec >= 1e9) { next.tv_sec++; next.tv_nsec -= 1e9; }
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
    }
    return NULL;
}

void* thread_control(void* arg) {
    struct timespec next;
    clock_gettime(CLOCK_MONOTONIC, &next);
    while (running) {
        struct timespec t0, t1;
        clock_gettime(CLOCK_MONOTONIC, &t0);

        SharedData d = get_shared_data();
        double v1 = d.ymx - d.y[0];
        double v2 = d.ymy - d.y[1];
        d.v[0] = d.alpha1 * v1;
        d.v[1] = d.alpha2 * v2;
        set_shared_data(d);

        clock_gettime(CLOCK_MONOTONIC, &t1);
        double dt = (t1.tv_sec - t0.tv_sec)*1e3 + (t1.tv_nsec - t0.tv_nsec)/1e6;
        if (exec_index[2] < 1000) exec_times[2][exec_index[2]++] = dt;

        next.tv_nsec += (long)(DT_CONTROL * 1e9);
        if (next.tv_nsec >= 1e9) { next.tv_sec++; next.tv_nsec -= 1e9; }
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
    }
    return NULL;
}
