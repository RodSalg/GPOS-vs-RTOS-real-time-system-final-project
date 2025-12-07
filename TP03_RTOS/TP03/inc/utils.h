#ifndef UTILS_H
#define UTILS_H
void* thread_logger_ui(void*);
void init_shared_data();
void stop_simulation();
void save_data_to_csv(const char* filename);
#endif
