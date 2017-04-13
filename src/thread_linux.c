#ifdef __linux__

#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#include <progbase/thread.h>

typedef void * (*pthread_func)(void *);

bool Thread_create(Thread * self, ThreadFunction func, void * arg) {
	return pthread_create(
		&(self->tid), 
		(const pthread_attr_t *)NULL, 
		(pthread_func)func, 
		arg) >= 0;
}

bool Thread_equal(Thread a, Thread b) {
	return pthread_equal(a.tid, b.tid);
}

Thread Thread_current(void) {
	return (Thread){
		.tid = pthread_self()
	};
}

void Thread_exit(int retval) {
	pthread_exit((void *)&retval);
}

bool Thread_detach(Thread self) {
	return pthread_cancel(self.tid) >= 0;
}

bool Thread_join(Thread self, int * res) {
	void ** result = NULL;
	if (pthread_join(self.tid, result) < 0) 
		return false;
	// cast pthread void * result to int
	*res = *(int *)result;
	return true;
}

/* MUTEX */

bool Mutex_init(Mutex * self, int type) {
	return pthread_mutex_init(&(self->pmutex), NULL) >= 0;
}

bool Mutex_lock(Mutex * self) {
	return pthread_mutex_lock(&(self->pmutex)) >= 0;
}

bool Mutex_unlock(Mutex * self) {
	return pthread_mutex_unlock(&(self->pmutex)) >= 0;
}

void Mutex_destroy(Mutex * self) {
	pthread_mutex_destroy(&self->pmutex);
}

#endif // __linux__
