#ifndef THREAD_H_INCLUDED
#define THREAD_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

// note: http://en.cppreference.com/w/c/thread

// types
typedef struct thread_s thread_t;
typedef void * (*thread_func_t)(void *);

// helper functions
thread_t * thread_current(void);
void thread_exit(void * retval);

// constructors
thread_t * thread_new(thread_func_t func);
thread_t * thread_newArgs(thread_func_t func, void * args);
// destructor
void thread_free(thread_t * self);

// methods
int thread_join(const thread_t * self);
int thread_join_result(const thread_t * self, void ** result);
int thread_terminate(const thread_t * self);
int thread_equals(const thread_t * self, const thread_t * obj);

/* MUTEX */

typedef struct mutex_s mutex_t;

mutex_t * mutex_new(void);
void mutex_free(mutex_t * self);

void mutex_lock(mutex_t * self);
void mutex_unlock(mutex_t * self);

#ifdef __cplusplus
}
#endif


#endif // THREAD_H_INCLUDED
