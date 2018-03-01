#include <progbase\thread.h>
#include <stdlib.h>
#include <stdbool.h>

// dummy implementation

bool Thread_create(Thread * self, ThreadFunction func, void * arg) {
	return false;
}

bool Thread_equal(Thread a, Thread b) {
		return false;
}

Thread Thread_current(void) {
	Thread temp = {0};
	return temp;
}

void Thread_exit(int retval) {
}

bool Thread_detach(Thread self) {
	return false;
}

bool Thread_join(Thread self, int * res) {
	return false;
}

bool Mutex_init(Mutex * self, int type) {
	return false;
}

bool Mutex_lock(Mutex * self) {
	return false;
}

bool Mutex_unlock(Mutex * self) {
	return false;
}

void Mutex_destroy(Mutex * self) {
}
