#define _XOPEN_SOURCE 700  // to enable struct timespec and clock_gettime
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <time.h>

#include <progbase.h>
#include <progbase/list.h>
#include <progbase/events.h>
#include <progbase/event_queue.h>
#include <progbase/clock.h>

struct ESObject {
	void * ref;  /**< a pointer to custom data */
	int refCount;  /**< reference counter */
	DestructorFunction destructor;  /**< a callback function pointer to free data */
};

ESObject * ESObject_new(void * ref, DestructorFunction destructor) {
	ESObject * self = malloc(sizeof(ESObject));
	self->refCount = 1;
	self->ref = ref;
	self->destructor = destructor;
	return self;
}

void * ESObject_ref(ESObject * self) {
	assert(self != NULL);
	return self->ref;
}

void ESObject_incref(ESObject * self) {
	assert(self != NULL);
	if (self->refCount > 0) {
		self->refCount++;
	}
} 

void ESObject_decref(ESObject * self) {
	assert(self != NULL);
	if (self->refCount > 0) {
		self->refCount--;
		if (0 == self->refCount) {
			// free
			if (self->destructor != NULL && self->ref != NULL) {
				self->destructor(self->ref);
			}
			free(self);
		}
	}
}

// Event

Event * Event_new(EventHandler * sender, int type, ESObject * payload) {
	Event * self = malloc(sizeof(struct Event));
	self->sender = sender;
	self->type = type;
	self->payload = payload;
	return self;
}

static void Event_free(Event * self) {
	if (self->payload) {
		ESObject_decref(self->payload);
	}
	free(self);
}

/* EventSystem */

/**
	@brief a structure that holds information about events and handlers
*/
struct EventSystem {
	List * handlers;  /**< a list of system event handlers */
	EventQueue * events;  /**< a a queue of unhandled events */
};

typedef enum {
	EventSystemActionContinue,
	EventSystemActionExit
} EventSystemAction;

/**
	@class EventHandlerEnumerator
	@brief an Enumerator (Iterator) to access List items of EventHandler type in EventSystem
*/
typedef struct EventHandlerEnumerator EventHandlerEnumerator;

EventSystemAction EventSystem_handleEvent(Event * event);
Event * EventSystem_getNextEvent(void);
EventHandlerEnumerator * EventSystem_getHandlers(void);
EventHandler * EventHandlerEnumerator_getNextHandler(EventHandlerEnumerator * self);
void EventHandlerEnumerator_free(EventHandlerEnumerator ** selfPtr);

typedef struct EventSystem EventSystem;

static EventSystem * g_eventSystem = &(EventSystem) { NULL, NULL };

static void EventHandler_handleEvent(EventHandler * self, Event * event);

EventHandler * EventHandler_new(ESObject * state, EventHandlerFunction handler) {
	EventHandler * self = malloc(sizeof(EventHandler));
	self->state = state;
	self->handler = handler; 
	return self;
}

static void EventHandler_free(EventHandler * self) {
	if (self->state) {
		ESObject_decref(self->state);
	}
	free(self);
}

static void EventHandler_handleEvent(EventHandler * self, Event * event) {
	self->handler(self, event);
}

/* EventHandlerEnumerator */

struct EventHandlerEnumerator {
	List * handlers;
	int index;
};

EventHandlerEnumerator * EventHandlerEnumerator_new(List * handlers) {
	EventHandlerEnumerator * self = malloc(sizeof(EventHandlerEnumerator));
	self->handlers = handlers;
	self->index = 0;
	return self;
}

void EventHandlerEnumerator_free(EventHandlerEnumerator ** selfPtr) {
	EventHandlerEnumerator * self = *selfPtr;
	free(self);
	*selfPtr = NULL;
}

EventHandler * EventHandlerEnumerator_getNextHandler(EventHandlerEnumerator * self) {
	if (self->index >= List_count(self->handlers)) return NULL;
	return List_get(self->handlers, self->index++);
}

Event * EventSystem_getNextEvent(void) {
	if (EventQueue_size(g_eventSystem->events) == 0) return NULL;
	return EventQueue_dequeue(g_eventSystem->events);
}

EventHandlerEnumerator * EventSystem_getHandlers(void) {
	return EventHandlerEnumerator_new(g_eventSystem->handlers);
}

/* EventSystem implementations */

enum {
	RemoveHandlerEventTypeId = (-2147483648) + 3,
	BreakLoopEventTypeId
};

EventSystemAction EventSystem_handleEvent(Event * event) {
	if (event->type == BreakLoopEventTypeId) {
		return EventSystemActionExit;
	}
	if (event->type == RemoveHandlerEventTypeId) {
		EventHandler * handler = ESObject_ref(event->payload);
		if (handler != NULL) {
			List_remove(g_eventSystem->handlers, handler);
			EventHandler_free(handler);
		}
	}
	return EventSystemActionContinue;
}

void EventSystem_addHandler(EventHandler * handler) {
	List_add(g_eventSystem->handlers, handler);
}

void EventSystem_removeHandler(EventHandler * handler) {
	EventSystem_emit(Event_new(
		NULL, 
		RemoveHandlerEventTypeId, 
		ESObject_new(handler, NULL)));
}

void EventSystem_raiseEvent(Event * event) {
	EventQueue_enqueue(g_eventSystem->events, event);
}

void EventSystem_emit(Event * event) {
	EventQueue_enqueue(g_eventSystem->events, event);
}

void EventSystem_init(void) {
	g_eventSystem->handlers = List_new();
	g_eventSystem->events = EventQueue_new();
}

void EventSystem_cleanup(void) {
	while (EventQueue_size(g_eventSystem->events) > 0) {
		Event * event = EventQueue_dequeue(g_eventSystem->events);
		Event_free(event);
	}
	EventQueue_free(&g_eventSystem->events);
	for (int i = 0; i < List_count(g_eventSystem->handlers); i++) {
		EventHandler * handler = List_get(g_eventSystem->handlers, i);
		EventHandler_free(handler);
	}
	List_free(&g_eventSystem->handlers);
}

void EventSystem_loop(void) {
	const int FPS = 30;
    const double millisPerFrame = 1000 / FPS;

	EventSystem_emit(Event_new(NULL, StartEventTypeId, NULL));
	double elapsedMillis = 0;
	Clock lastTicks = Clock_now();
	bool isRunning = true;
	while (isRunning) {
		Clock current = Clock_now();
        elapsedMillis = Clock_diffMillis(current, lastTicks);

		ESObject * o = ESObject_new(&elapsedMillis, NULL);
		EventSystem_emit(Event_new(NULL, UpdateEventTypeId, o));
		
		Event * event = NULL;
		while((event = EventSystem_getNextEvent()) != NULL) {
			if (EventSystem_handleEvent(event) == EventSystemActionExit) {
				isRunning = false;
				EventSystem_emit(Event_new(NULL, ExitEventTypeId, NULL));
			} else {
				EventHandlerEnumerator * handlersEnum = EventSystem_getHandlers();
				EventHandler * handler = NULL;
				while((handler = EventHandlerEnumerator_getNextHandler(handlersEnum)) != NULL) {
					EventHandler_handleEvent(handler, event);
				}
				EventHandlerEnumerator_free(&handlersEnum);
			}
			Event_free(event);
		}

		double millis = Clock_diffMillis(Clock_now(), current);
        if (millis < millisPerFrame) sleepMillis(millisPerFrame - millis);
        lastTicks = current;
	}
}

void EventSystem_exit(void) {
	EventSystem_emit(Event_new(NULL, BreakLoopEventTypeId, NULL));
}

double UpdateEvent_elapsedMillis(Event * event) {
	return *(double *)ESObject_ref(event->payload);
}
