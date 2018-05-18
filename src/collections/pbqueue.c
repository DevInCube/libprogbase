#include <stdlib.h>

#include <progbase/collections/pblist.h>
#include <progbase/collections/pbqueue.h>

struct PbQueue {
	PbList * list;
};

PbQueue * PbQueue_new(void) {
	PbQueue * self = malloc(sizeof(PbQueue));
	self->list = PbList_new();
	return self;
}

void PbQueue_free(PbQueue ** selfPtr) {
	PbQueue * self = *selfPtr;
	PbList_free(self->list);
	free(self);
	*selfPtr = NULL;
}

void PbQueue_enqueue(PbQueue *  self, void * value) {
	PbList_add(self->list, value);
}

void * PbQueue_dequeue(PbQueue *  self) {
	void * value = PbList_at(self->list, 0);
	PbList_removeAt(self->list, 0);
	return value;
}

int PbQueue_size(PbQueue * self) {
	return PbList_count(self->list);
} 