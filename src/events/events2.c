#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <progbase.h>
#include <progbase/events2.h> 
#include <progbase/clock.h>
#include <progbase/collections/enumerator.h>
#include <progbase/collections/list.h>
#include <progbase/collections/queue.h>

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
	Queue * events;  /**< a a queue of unhandled events */
}; 

typedef enum {
	EventSystemActionContinue,
	EventSystemActionExit
} EventSystemAction;

EventSystemAction EventSystem_handleEvent(Event * event);
Enumerator * EventSystem_getHandlers(void);

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

Enumerator * EventSystem_getHandlers(void) {
	return List_getNewEnumerator(g_eventSystem->handlers);
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

Event * EventSystem_getNextEvent(void) {
	if (Queue_size(g_eventSystem->events) == 0) return NULL;
	return Queue_dequeue(g_eventSystem->events);
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
		Event_free(event);
	}
	Queue_free(&g_eventSystem->events);
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
				Enumerator * handlersEnum = EventSystem_getHandlers();
				EventHandler * handler = NULL;
				while(Enumerator_moveNext(handlersEnum)) {
					handler = Enumerator_current(handlersEnum);
					EventHandler_handleEvent(handler, event);
				}
				Enumerator_free(handlersEnum);
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
