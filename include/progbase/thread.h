/** @file
    @brief Threading module
*/

#pragma once

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

// note: http://en.cppreference.com/w/c/thread

// types
typedef struct Thread Thread;
struct Thread {
    pthread_t tid;
};

typedef void * (*ThreadFunction)(void *);

bool Thread_create(Thread * self, ThreadFunction func, void * arg);
bool Thread_equal(Thread a, Thread b);
Thread Thread_current(void);
void Thread_exit(int retval);
bool Thread_detach(Thread self);
bool Thread_join(Thread self, int * res);

/* MUTEX */

typedef struct Mutex Mutex;
struct Mutex {
    pthread_mutex_t pmutex;
};

bool Mutex_init(Mutex * self, int type);
bool Mutex_lock(Mutex * self);
bool Mutex_unlock(Mutex * self);
void Mutex_destroy(Mutex * self);

#ifdef __cplusplus
}
#endif
