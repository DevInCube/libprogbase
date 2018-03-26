#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <progbase.h>
#include <progbase/events.h> 
#include <progbase/clock.h>
#include <progbase/collections/enumerator.h>
#include <progbase/collections/list.h>
#include <progbase/collections/queue.h>

void Event_free(Event ** dataPtr);

Event * Event_new(EventHandler * sender, int type, void * data, DestructorFunction dest) {
	Event * self = malloc(sizeof(struct Event));
	self->sender = sender;
	self->type = type;
	self->data = data;
	self->destructor = dest;
	return self;
}

void Event_free(Event ** selfPtr) {
	Event * self = *selfPtr;
	if (self->destructor != NULL && self->data != NULL) {
		self->destructor(self->data);
	}
	free(self);
	*selfPtr = NULL;
}

/**
	@brief a structure that holds information about events and handlers
*/
struct EventSystem {
	List * handlers;  /**< a list of system event handlers */
	Queue * events;  /**< a a queue of unhandled events */
};

/* EventSystem */

typedef enum {
	EventSystemActionContinue,
	EventSystemActionExit
} EventSystemAction;

bool EventSystem_handleEvent(Event * event);
Event * EventSystem_getNextEvent(void);
Enumerator * EventSystem_getHandlers(void);

typedef struct EventSystem EventSystem;

static EventSystem * g_eventSystem = &(EventSystem) { NULL, NULL };

void EventHandler_handleEvent(EventHandler * self, Event * event);

EventHandler * EventHandler_new(void * data, DestructorFunction dest, EventHandlerFunction handler) {
	if (data != NULL && dest == NULL) assert(0 && "destructor for non-null data is null");
	EventHandler * self = malloc(sizeof(EventHandler));
	self->data = data;
	self->destructor = dest;
	self->handler = handler; 
	self->_refCount = 1;
	return self;
}

void EventHandler_incref(EventHandler * self) {
	self->_refCount++;
}

void EventHandler_decref(EventHandler * self) {
	self->_refCount--;
	if (0 == self->_refCount) {
		// free
		if (self->destructor != NULL && self->data != NULL) {
			self->destructor(self->data);
		}
		free(self);
	}
}

void EventHandler_handleEvent(EventHandler * self, Event * event) {
	self->handler(self, event);
}

Event * EventSystem_getNextEvent(void) {
	if (Queue_size(g_eventSystem->events) == 0) return NULL;
	return Queue_dequeue(g_eventSystem->events);
}

Enumerator * EventSystem_getHandlers(void) {
	return List_getNewEnumerator(g_eventSystem->handlers);
}

/* EventSystem implementations */

enum {
	RemoveHandlerEventTypeId = ExitEventTypeId + 1,
	BreakLoopEventTypeId
};

bool EventSystem_handleEvent(Event * event) {
	if (event->type == BreakLoopEventTypeId) {
		return EventSystemActionExit;
	}
	if (event->type == RemoveHandlerEventTypeId) {
		EventHandler * handler = event->data;
		if (handler != NULL) {
			List_remove(g_eventSystem->handlers, handler);
			EventHandler_decref(handler);
		}
	}
	return EventSystemActionContinue;
}

void EventSystem_addHandler(EventHandler * handler) {
	List_add(g_eventSystem->handlers, handler);
}

void EventSystem_removeHandler(EventHandler * handler) {
	EventSystem_emit(Event_new(NULL, RemoveHandlerEventTypeId, handler, NULL));
}

void EventSystem_raiseEvent(Event * event) {
	Queue_enqueue(g_eventSystem->events, event);
}

void EventSystem_emit(Event * event) {
	Queue_enqueue(g_eventSystem->events, event);
}

void EventSystem_init(void) {
	g_eventSystem->handlers = List_new();
	g_eventSystem->events = Queue_new();
}

void EventSystem_cleanup(void) {
	while (Queue_size(g_eventSystem->events) > 0) {
		Event * event = Queue_dequeue(g_eventSystem->events);
		Event_free(&event);
	}
	Queue_free(&g_eventSystem->events);
	for (int i = 0; i < List_count(g_eventSystem->handlers); i++) {
		EventHandler * handler = List_get(g_eventSystem->handlers, i);
		EventHandler_decref(handler);
	}
	List_free(&g_eventSystem->handlers);
}

void EventSystem_loop(void) {
	const int FPS = 30;
    const double millisPerFrame = 1000 / FPS;

	EventSystem_emit(Event_new(NULL, StartEventTypeId, NULL, NULL));
	double elapsedMillis = 0;
	Clock lastTicks = Clock_now();
	bool isRunning = true;
	while (isRunning) {
		Clock current = Clock_now();
        elapsedMillis = Clock_diffMillis(current, lastTicks);

		EventSystem_emit(Event_new(NULL, UpdateEventTypeId, &elapsedMillis, NULL));
		
		Event * event = NULL;
		while((event = EventSystem_getNextEvent()) != NULL) {
			if (EventSystem_handleEvent(event) == EventSystemActionExit) {
				isRunning = false;
				EventSystem_emit(Event_new(NULL, ExitEventTypeId, NULL, NULL));
			} else {
				Enumerator * handlersEnum = EventSystem_getHandlers();
				EventHandler * handler = NULL;
				while(Enumerator_moveNext(handlersEnum)) {
					handler = Enumerator_current(handlersEnum);
					EventHandler_handleEvent(handler, event);
				}
				Enumerator_free(handlersEnum);
			}
			Event_free(&event);
		}

		double millis = Clock_diffMillis(Clock_now(), current);
        if (millis < millisPerFrame) sleepMillis(millisPerFrame - millis);
        lastTicks = current;
	}
}

void EventSystem_exit(void) {
	EventSystem_emit(Event_new(NULL, BreakLoopEventTypeId, NULL, NULL));
}
