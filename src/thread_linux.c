#ifdef __linux__

#include <stdlib.h>
#include <pthread.h>

#include <progbase/thread.h>

struct thread_s {
    pthread_t tid;
};

static thread_t * __thread_new(void) {
    return malloc(sizeof(struct thread_s));
}

// helper functions
thread_t * thread_current(void) {
    thread_t * thread = __thread_new();
    thread->tid = pthread_self();
    return thread;
}

void thread_exit(void * retval) {
    pthread_exit(retval);
}

// constructors
thread_t * thread_new(thread_func_t func) {
    return thread_newArgs(func, NULL);
}

thread_t * thread_newArgs(thread_func_t func, void * args) {
    thread_t * thread = __thread_new();
    int err = pthread_create(&(thread->tid), (const pthread_attr_t *)NULL, func, args);
    if (0 != err) {
        return NULL;  // @todo
    }
    return thread;
}

// destructor
void thread_free(thread_t * self) {
    free(self);
}

// methods
int thread_join(const thread_t * self) {
    return pthread_join(self->tid, NULL);
}

int thread_join_result(const thread_t * self, void ** result) {
    return pthread_join(self->tid, result);
}

int thread_terminate(const thread_t * self) {
    return pthread_cancel(self->tid);
}

int thread_equals(const thread_t * self, const thread_t * obj) {
    return pthread_equal(self->tid, obj->tid);
}

/* MUTEX */

struct mutex_s {
    pthread_mutex_t pmutex;
};

mutex_t * mutex_new(void) {
    mutex_t * mutex = malloc(sizeof(struct mutex_s));
    pthread_mutex_init(&(mutex->pmutex), NULL);
    return mutex;
}

void mutex_free(mutex_t * self) {
    free(self);
}

void mutex_lock(mutex_t * self) {
    pthread_mutex_lock(&(self->pmutex));
}

void mutex_unlock(mutex_t * self) {
    pthread_mutex_unlock(&(self->pmutex));
}

#endif // __linux__
