// (modificado) lab03_robot_sim/src/utils.c
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "shared.h"

volatile bool running = true;
volatile double exec_times[7][1000]; // tempo de execucao por thread
volatile int exec_index[7] = {0};

void stop_simulation() {
    running = false;
}

void init_shared_data() {
    init_shared(); // defined in shared.c
}

void* thread_logger_ui(void* arg) {
    FILE* file = fopen("data/log.txt", "w");
    struct timespec next;
    clock_gettime(CLOCK_MONOTONIC, &next);

    printf("Digite novos valores de alpha1 e alpha2 (ex: 2.5 3.2), ou ENTER para manter:\n");
    while (running) {
        SharedData d = get_shared_data();
        fprintf(file, "%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", d.x[0], d.x[1], d.x[2], d.y[0], d.y[1], d.t, d.xref, d.yref);
        printf("t=%.2f | pos=(%.2f, %.2f) | theta=%.2f | ref=(%.2f, %.2f) | alpha1=%.2f alpha2=%.2f\n",
               d.t, d.x[0], d.x[1], d.x[2], d.xref, d.yref, d.alpha1, d.alpha2);
        fflush(file);

        char input[64];
        if (fgets(input, sizeof(input), stdin) && strlen(input) > 1) {
            double a1, a2;
            if (sscanf(input, "%lf %lf", &a1, &a2) == 2) {
                d.alpha1 = a1;
                d.alpha2 = a2;
                set_shared_data(d);
                printf("Novos alphas definidos: alpha1=%.2f alpha2=%.2f\n", a1, a2);
            }
        }

        next.tv_nsec += 100 * 1e6;
        if (next.tv_nsec >= 1e9) {
            next.tv_sec++;
            next.tv_nsec -= 1e9;
        }
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
    }
    fclose(file);
    return NULL;
}

void save_data_to_csv(const char* filename) {
    FILE* in = fopen("data/log.txt", "r");
    FILE* out = fopen(filename, "w");
    char line[256];
    fprintf(out, "x1,x2,x3,y1,y2,t,xref,yref\n");
    while (fgets(line, sizeof(line), in)) fputs(line, out);
    fclose(in); fclose(out);

    // Salva tempos de execução
    FILE* stats = fopen("data/times.csv", "w");
    fprintf(stats, "thread_id,time_ms\n");
    for (int t = 0; t < 7; ++t)
        for (int i = 0; i < exec_index[t]; ++i)
            fprintf(stats, "%d,%.4lf\n", t, exec_times[t][i]);
    fclose(stats);
}