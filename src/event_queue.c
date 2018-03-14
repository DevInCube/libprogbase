#include <stdlib.h>

#include <progbase/list.h>
#include <progbase/events.h>
#include <progbase/event_queue.h>


/**
	@struct EventQueue
	@brief a queue of items of type Event
*/
struct EventQueue {
	List * list;
};

EventQueue * EventQueue_new(void) {
	EventQueue * self = malloc(sizeof(EventQueue));
	self->list = List_new();
	return self;
}

void EventQueue_free(EventQueue ** selfPtr) {
	EventQueue * self = *selfPtr;
	List_free(&self->list);
	free(self);
	*selfPtr = NULL;
}

/**
	@brief adds new Event to the EventQueue tail
	@param event - new event
*/
void EventQueue_enqueue(EventQueue *  self, Event * event) {
	List_add(self->list, event);
}

/**
	@brief removes a n Event from the EventQueue head
	@returns Event from head or NULL otherwise
*/
Event * EventQueue_dequeue(EventQueue *  self) {
	Event * event = List_get(self->list, 0);
	List_removeAt(self->list, 0);
	return event;
}

/**
	@returns the number of events in EventQueue
*/
int EventQueue_size(EventQueue * self) {
	return List_count(self->list);
}