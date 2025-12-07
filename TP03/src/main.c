#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include "robot.h"
#include "control.h"
#include "reference.h"
#include "utils.h"

#define SIM_DURATION 20.0

int main() {
    init_shared_data();

    pthread_t threads[7];
    pthread_create(&threads[0], NULL, thread_robot_sim, NULL);
    pthread_create(&threads[1], NULL, thread_linearization, NULL);
    pthread_create(&threads[2], NULL, thread_control, NULL);
    pthread_create(&threads[3], NULL, thread_ref_model_x, NULL);
    pthread_create(&threads[4], NULL, thread_ref_model_y, NULL);
    pthread_create(&threads[5], NULL, thread_reference_gen, NULL);
    pthread_create(&threads[6], NULL, thread_logger_ui, NULL);

    sleep((int)SIM_DURATION);
    stop_simulation();

    for (int i = 0; i < 7; ++i) pthread_join(threads[i], NULL);
    save_data_to_csv("data/output.csv");
    return 0;
}