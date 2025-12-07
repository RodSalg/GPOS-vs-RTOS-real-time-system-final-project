#include "robot.h"
#include "shared.h"
#include <math.h>
#include <time.h>

#define R 0.3
#define DT 0.03

void* thread_robot_sim(void* arg) {
    struct timespec next;
    clock_gettime(CLOCK_MONOTONIC, &next);

    while (running) {
        struct timespec t0, t1;
        clock_gettime(CLOCK_MONOTONIC, &t0);

        SharedData d = get_shared_data();
        double v = d.u[0], w = d.u[1];
        double dx = cos(d.x[2]) * v;
        double dy = sin(d.x[2]) * v;
        double dtheta = w;
        d.x[0] += dx * DT;
        d.x[1] += dy * DT;
        d.x[2] += dtheta * DT;
        d.y[0] = d.x[0] + R * cos(d.x[2]);
        d.y[1] = d.x[1] + R * sin(d.x[2]);
        d.t += DT;
        set_shared_data(d);

        clock_gettime(CLOCK_MONOTONIC, &t1);
        double dt = (t1.tv_sec - t0.tv_sec)*1e3 + (t1.tv_nsec - t0.tv_nsec)/1e6;
        if (exec_index[0] < 1000) exec_times[0][exec_index[0]++] = dt;

        next.tv_nsec += (long)(DT * 1e9);
        if (next.tv_nsec >= 1e9) { next.tv_sec++; next.tv_nsec -= 1e9; }
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
    }
    return NULL;
}