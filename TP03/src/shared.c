#include "shared.h"
#include <string.h>
#include <stdbool.h>

static SharedData shared;
volatile int step_count = 0;

void init_shared() {
    memset(&shared, 0, sizeof(SharedData));
    pthread_mutex_init(&shared.lock, NULL);
    shared.alpha1 = 3.0;
    shared.alpha2 = 3.0;
}

SharedData get_shared_data() {
    pthread_mutex_lock(&shared.lock);
    SharedData copy = shared;
    pthread_mutex_unlock(&shared.lock);
    return copy;
}

void set_shared_data(SharedData new_data) {
    pthread_mutex_lock(&shared.lock);
    shared = new_data;
    pthread_mutex_unlock(&shared.lock);
}
