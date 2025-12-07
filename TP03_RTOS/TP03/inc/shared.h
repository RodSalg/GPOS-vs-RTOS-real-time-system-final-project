#ifndef SHARED_H
#define SHARED_H
#include <pthread.h>
#include <stdbool.h>

typedef struct {
    double x[3];
    double y[2];
    double u[2];
    double v[2];
    double ymx, ymy;
    double xref, yref;
    double t;
    double alpha1, alpha2;
    pthread_mutex_t lock;
} SharedData;

void init_shared();
SharedData get_shared_data();
void set_shared_data(SharedData);
extern volatile int step_count;
extern volatile int exec_index[];
extern volatile double exec_times[][1000];
extern volatile bool running;

#endif