#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <sched.h>      // Para políticas de escalonamento e prioridades
#include <sys/mman.h>   // Para mlockall

#include "robot.h"
#include "control.h"
#include "reference.h"
#include "utils.h"

#define SIM_DURATION 20.0

// --- PRIORIDADES DE TEMPO REAL (RMA: Período mais curto -> Maior prioridade) ---
// O kernel RT do Linux usa prioridades de 1 (baixa) a 99 (alta).
#define RT_PRIORITY_MAX     99
#define RT_PRIORITY_MIN     1

#define ROBOT_SIM_PRIORITY      (RT_PRIORITY_MAX - 1)  // Período: 30ms
#define LINEARIZATION_PRIORITY  (RT_PRIORITY_MAX - 2)  // Período: 40ms
#define CONTROL_PRIORITY        (RT_PRIORITY_MAX - 3)  // Período: 50ms
#define REF_MODEL_PRIORITY      (RT_PRIORITY_MAX - 3)  // Período: 50ms
#define LOGGER_UI_PRIORITY      (RT_PRIORITY_MAX - 4)  // Período: 100ms
#define REFERENCE_GEN_PRIORITY  (RT_PRIORITY_MAX - 5)  // Período: 120ms

// Função auxiliar para criar threads com prioridade de tempo real
void create_rt_thread(pthread_t *thread, void *(*start_routine) (void *), int priority) {
    pthread_attr_t attr;
    struct sched_param param;

    pthread_attr_init(&attr);

    // Configura a thread para usar os parâmetros de escalonamento definidos explicitamente
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);

    // Define a política de escalonamento para tempo real (First-In, First-Out)
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);

    // Define a prioridade da thread
    param.sched_priority = priority;
    pthread_attr_setschedparam(&attr, &param);

    // Cria a thread com os atributos definidos
    pthread_create(thread, &attr, start_routine, NULL);

    pthread_attr_destroy(&attr);
}

int main() {
    // --- CONFIGURAÇÃO DE TEMPO REAL PARA O PROCESSO PRINCIPAL ---
    // Trava a memória do processo (atual e futura) para prevenir page faults, que causam latência.
    // Requer execução com privilégios (sudo).
    if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1) {
        perror("mlockall failed. Try running with sudo.");
        exit(EXIT_FAILURE);
    }
    printf("Process memory locked successfully.\n");

    init_shared_data();

    pthread_t threads[7];
    
    printf("Creating real-time threads...\n");
    // Cria as threads usando a função auxiliar com suas respectivas prioridades
    create_rt_thread(&threads[0], thread_robot_sim, ROBOT_SIM_PRIORITY);
    create_rt_thread(&threads[1], thread_linearization, LINEARIZATION_PRIORITY);
    create_rt_thread(&threads[2], thread_control, CONTROL_PRIORITY);
    create_rt_thread(&threads[3], thread_ref_model_x, REF_MODEL_PRIORITY);
    create_rt_thread(&threads[4], thread_ref_model_y, REF_MODEL_PRIORITY);
    create_rt_thread(&threads[5], thread_reference_gen, REFERENCE_GEN_PRIORITY);
    create_rt_thread(&threads[6], thread_logger_ui, LOGGER_UI_PRIORITY);

    printf("Simulation running for %.1f seconds...\n", SIM_DURATION);
    sleep((int)SIM_DURATION);
    stop_simulation();

    printf("Joining threads...\n");
    for (int i = 0; i < 7; ++i) {
        pthread_join(threads[i], NULL);
    }

    printf("Saving data...\n");
    save_data_to_csv("data/output_rt.csv");
    printf("Simulation finished.\n");

    return 0;
}