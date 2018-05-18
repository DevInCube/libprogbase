#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <progbase.h>
#include <progbase/events2.h> 
#include <progbase/clock.h>
#include <progbase/collections/pbenumerator.h>
#include <progbase/collections/pblist.h>
#include <progbase/collections/pbqueue.h>

#define foreach(VARTYPE, VARNAME, LIST)   \
   for (int VARNAME##_i = 0; !VARNAME##_i; ) \
       for (PbEnumerator * VARNAME##_e = PbList_getNewPbEnumerator(LIST); \
            !VARNAME##_i; \
            ++VARNAME##_i, PbEnumerator_free(VARNAME##_e))\
         for (VARTYPE VARNAME = NULL; \
             (VARNAME##_i = PbEnumerator_moveNext(VARNAME##_e), VARNAME = PbEnumerator_current(VARNAME##_e), VARNAME##_i); \
             )

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
	PbList * handlers;  /**< a list of system event handlers */
	PbQueue * events;  /**< a a queue of unhandled events */
}; 

typedef enum {
	EventSystemActionContinue,
	EventSystemActionExit
} EventSystemAction;

static EventSystemAction EventSystem_handleEvent(Event * event);

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

/* EventSystem implementations */

enum {
	RemoveHandlerEventTypeId = INT_MIN + 3,
	BreakLoopEventTypeId
};

static EventSystemAction EventSystem_handleEvent(Event * event) {
	if (event->type == BreakLoopEventTypeId) {
		return EventSystemActionExit;
	}
	if (event->type == RemoveHandlerEventTypeId) {
		EventHandler * handler = ESObject_ref(event->payload);
		if (handler != NULL) {
			PbList_remove(g_eventSystem->handlers, handler);
			EventHandler_free(handler);
		}
	}
	return EventSystemActionContinue;
}

static Event * EventSystem_dequeueNextEvent(void) {
	return PbQueue_size(g_eventSystem->events) != 0
		? PbQueue_dequeue(g_eventSystem->events)
		: NULL;
}

void EventSystem_addHandler(EventHandler * handler) {
	PbList_add(g_eventSystem->handlers, handler);
}

void EventSystem_removeHandler(EventHandler * handler) {
	EventSystem_emit(Event_new(
		NULL, 
		RemoveHandlerEventTypeId, 
		ESObject_new(handler, NULL)));
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
    Event * event = NULL;
	while ((event = EventSystem_dequeueNextEvent()) != NULL) {
		Event_free(event);
	}
	PbQueue_free(&g_eventSystem->events);
	for (int i = 0; i < PbList_count(g_eventSystem->handlers); i++) {
		EventHandler * handler = PbList_at(g_eventSystem->handlers, i);
		EventHandler_free(handler);
	}
	PbList_free(g_eventSystem->handlers);
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
		while((event = EventSystem_dequeueNextEvent()) != NULL) {
			if (EventSystem_handleEvent(event) == EventSystemActionExit) {
				isRunning = false;
				EventSystem_emit(Event_new(NULL, ExitEventTypeId, NULL));
			} else {
				foreach(EventHandler *, handler, g_eventSystem->handlers) {
					EventHandler_handleEvent(handler, event);
				}
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
