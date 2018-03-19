#include <stdlib.h>

#include <progbase/collections/list.h>
#include <progbase/collections/queue.h>

struct Queue {
	List * list;
};

Queue * Queue_new(void) {
	Queue * self = malloc(sizeof(Queue));
	self->list = List_new();
	return self;
}

void Queue_free(Queue ** selfPtr) {
	Queue * self = *selfPtr;
	List_free(&self->list);
	free(self);
	*selfPtr = NULL;
}

void Queue_enqueue(Queue *  self, void * value) {
	List_add(self->list, value);
}

void * Queue_dequeue(Queue *  self) {
	void * value = List_get(self->list, 0);
	List_removeAt(self->list, 0);
	return value;
}

int Queue_size(Queue * self) {
	return List_count(self->list);
} 