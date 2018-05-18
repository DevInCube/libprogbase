#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <progbase.h>
#include <progbase/events.h> 
#include <progbase/clock.h>
#include <progbase/collections/pbenumerator.h>
#include <progbase/collections/pblist.h>
#include <progbase/collections/pbqueue.h>

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
	PbList * handlers;  /**< a list of system event handlers */
	PbQueue * events;  /**< a a queue of unhandled events */
};

/* EventSystem */

typedef enum {
	EventSystemActionContinue,
	EventSystemActionExit
} EventSystemAction;

bool EventSystem_handleEvent(Event * event);
Event * EventSystem_getNextEvent(void);
PbEnumerator * EventSystem_getHandlers(void);

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
	if (PbQueue_size(g_eventSystem->events) == 0) return NULL;
	return PbQueue_dequeue(g_eventSystem->events);
}

PbEnumerator * EventSystem_getHandlers(void) {
	return PbList_getNewPbEnumerator(g_eventSystem->handlers);
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
			PbList_remove(g_eventSystem->handlers, handler);
			EventHandler_decref(handler);
		}
	}
	return EventSystemActionContinue;
}

void EventSystem_addHandler(EventHandler * handler) {
	PbList_add(g_eventSystem->handlers, handler);
}

void EventSystem_removeHandler(EventHandler * handler) {
	EventSystem_emit(Event_new(NULL, RemoveHandlerEventTypeId, handler, NULL));
}

void EventSystem_raiseEvent(Event * event) {
	PbQueue_enqueue(g_eventSystem->events, event);
}

void EventSystem_emit(Event * event) {
	PbQueue_enqueue(g_eventSystem->events, event);
}

void EventSystem_init(void) {
	g_eventSystem->handlers = PbList_new();
	g_eventSystem->events = PbQueue_new();
}

void EventSystem_cleanup(void) {
	while (PbQueue_size(g_eventSystem->events) > 0) {
		Event * event = PbQueue_dequeue(g_eventSystem->events);
		Event_free(&event);
	}
	PbQueue_free(&g_eventSystem->events);
	for (int i = 0; i < PbList_count(g_eventSystem->handlers); i++) {
		EventHandler * handler = PbList_at(g_eventSystem->handlers, i);
		EventHandler_decref(handler);
	}
	PbList_free(g_eventSystem->handlers);
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
				PbEnumerator * handlersEnum = EventSystem_getHandlers();
				EventHandler * handler = NULL;
				while(PbEnumerator_moveNext(handlersEnum)) {
					handler = PbEnumerator_current(handlersEnum);
					EventHandler_handleEvent(handler, event);
				}
				PbEnumerator_free(handlersEnum);
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
