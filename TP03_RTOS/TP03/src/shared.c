#define _GNU_SOURCE
#include "shared.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

static SharedData shared;
volatile int step_count = 0;

void init_shared() {
    memset(&shared, 0, sizeof(SharedData));

    // --- CONFIGURAÇÃO DO MUTEX PARA TEMPO REAL ---
    pthread_mutexattr_t mutex_attr;

    if (pthread_mutexattr_init(&mutex_attr) != 0) {
        perror("pthread_mutexattr_init");
    }

    // Agora o compilador irá reconhecer PTHREAD_PRIO_INHERIT
    if (pthread_mutexattr_setprotocol(&mutex_attr, PTHREAD_PRIO_INHERIT) != 0) {
        perror("pthread_mutexattr_setprotocol");
    }

    if (pthread_mutex_init(&shared.lock, &mutex_attr) != 0) {
        perror("pthread_mutex_init");
    }

    pthread_mutexattr_destroy(&mutex_attr);

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