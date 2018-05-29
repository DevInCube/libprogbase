#include <stdlib.h>

#include "pbvector.h"
#include "pbqueue.h"

struct PbQueue {
	PbVector * list;
};

PbQueue * PbQueue_new(void) {
	PbQueue * self = malloc(sizeof(PbQueue));
	self->list = PbVector_new();
	return self;
}

void PbQueue_free(PbQueue ** selfPtr) {
	PbQueue * self = *selfPtr;
	PbVector_free(self->list);
	free(self);
	*selfPtr = NULL;
}

void PbQueue_enqueue(PbQueue *  self, void * value) {
	PbVector_add(self->list, value);
}

void * PbQueue_dequeue(PbQueue *  self) {
	void * value = PbVector_at(self->list, 0);
	PbVector_removeAt(self->list, 0);
	return value;
}

int PbQueue_size(PbQueue * self) {
	return PbVector_count(self->list);
} 