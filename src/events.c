#define _XOPEN_SOURCE 700  // to enable struct timespec and clock_gettime
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <time.h>

#include <progbase.h>
#include <progbase/list.h>
#include <progbase/events.h>
#include <progbase/clock.h>

struct __EventSystemObjectPrivate {
	int refCount;  /**< private reference counter */
	void * data;  /**< a copy pointer to public data */
	DestructorFunction destructor;  /**< a callback function pointer to call to free data */
};

/* Event Queue */

typedef struct EventQueue EventQueue;

EventQueue * EventQueue_new(void);
void EventQueue_free(EventQueue ** selfPtr);
void EventQueue_enqueue(EventQueue *  self, Event * event);
Event * EventQueue_dequeue(EventQueue *  self);
int EventQueue_size(EventQueue * self);

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

EventSystemObjectPrivate * Private_new(void * data, DestructorFunction destructor) {
	EventSystemObjectPrivate * self = malloc(sizeof(EventSystemObjectPrivate));
	self->refCount = 1;
	self->data = data;
	self->destructor = destructor;
	return self;
}

Event * Event_new(EventHandler * sender, int type, void * data, DestructorFunction dest) {
	Event * self = malloc(sizeof(struct Event));
	self->_private = Private_new(data, dest);
	self->sender = sender;
	self->type = type;
	self->data = data;
	return self;
}

/**
	@brief a structure that holds information about events and handlers
*/
struct EventSystem {
	List * handlers;  /**< a list of system event handlers */
	EventQueue * events;  /**< a a queue of unhandled events */
};

/* EventSystem */

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

void EventHandler_handleEvent(EventHandler * self, Event * event);

EventHandler * EventHandler_new(void * data, DestructorFunction dest, EventHandlerFunction handler) {
	EventHandler * self = malloc(sizeof(EventHandler));
	self->_private = Private_new(data, dest);
	self->data = data;
	self->handler = handler; 
	return self;
}

void EventSystem_incref(void * self) {
	EventSystemObjectPrivate ** privPtr = (EventSystemObjectPrivate**)self;
	EventSystemObjectPrivate * priv = *privPtr;
	priv->refCount++;
} 

void EventSystem_decref(void * self) {
	EventSystemObjectPrivate ** privPtr = (EventSystemObjectPrivate**)self;
	EventSystemObjectPrivate * priv = *privPtr;
	priv->refCount--;
	if (0 == priv->refCount) {
		// free
		if (priv->destructor != NULL && priv->data != NULL) {
			priv->destructor(priv->data);
		}
		free(priv);
		free(self);
	}
}

void EventHandler_handleEvent(EventHandler * self, Event * event) {
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
		EventHandler * handler = event->data;
		if (handler != NULL) {
			List_remove(g_eventSystem->handlers, handler);
			EventSystem_decref(handler);
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
		EventSystem_decref(event);
	}
	EventQueue_free(&g_eventSystem->events);
	for (int i = 0; i < List_count(g_eventSystem->handlers); i++) {
		EventHandler * handler = List_get(g_eventSystem->handlers, i);
		EventSystem_decref(handler);
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
				EventHandlerEnumerator * handlersEnum = EventSystem_getHandlers();
				EventHandler * handler = NULL;
				while((handler = EventHandlerEnumerator_getNextHandler(handlersEnum)) != NULL) {
					EventHandler_handleEvent(handler, event);
				}
				EventHandlerEnumerator_free(&handlersEnum);
			}
			EventSystem_decref(event);
		}

		double millis = Clock_diffMillis(Clock_now(), current);
        if (millis < millisPerFrame) sleepMillis(millisPerFrame - millis);
        lastTicks = current;
	}
}

void EventSystem_exit(void) {
	EventSystem_emit(Event_new(NULL, BreakLoopEventTypeId, NULL, NULL));
}

double UpdateEvent_elapsedMillis(Event * event) {
	return *(double *)event->data;
}
