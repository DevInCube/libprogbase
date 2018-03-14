#define _XOPEN_SOURCE 700  // to enable struct timespec and clock_gettime
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <time.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#include <sys/timeb.h>
	#include <sys/types.h>
	#include <winsock2.h>

	#define CLOCK_REALTIME 0 
	int clock_gettime(int X, struct timespec *tv);
#endif

#include <progbase.h>
#include <progbase/list.h>
#include <progbase/events.h>

void Event_free(Event ** dataPtr);

/**
    @struct Clock
    @brief encapsulates clock data
*/
typedef struct Clock Clock;
struct Clock {
    struct timespec time;
};

Clock Clock_now(void);
double Clock_diffMillis(Clock c1, Clock c2);

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

bool EventSystem_handleEvent(Event * event);
Event * EventSystem_getNextEvent(void);
EventHandlerEnumerator * EventSystem_getHandlers(void);
EventHandler * EventHandlerEnumerator_getNextHandler(EventHandlerEnumerator * self);
void EventHandlerEnumerator_free(EventHandlerEnumerator ** selfPtr);

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
		Event_free(&event);
	}
	EventQueue_free(&g_eventSystem->events);
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
				EventHandlerEnumerator * handlersEnum = EventSystem_getHandlers();
				EventHandler * handler = NULL;
				while((handler = EventHandlerEnumerator_getNextHandler(handlersEnum)) != NULL) {
					EventHandler_handleEvent(handler, event);
				}
				EventHandlerEnumerator_free(&handlersEnum);
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

double UpdateEvent_elapsedMillis(Event * event) {
	return *(double *)event->data;
}

/* Clock implementations */

Clock Clock_now(void) {
    Clock clock;
    clock_gettime(CLOCK_REALTIME, &(clock.time));
    return clock;
}

static double _getMillis(struct timespec * t1, struct timespec * t2) {
    double ms1 = (double)t1->tv_sec * 1000 + (double)t1->tv_nsec / 1000000.0;
    double ms2 = (double)t2->tv_sec * 1000 + (double)t2->tv_nsec / 1000000.0;
    return ms1 - ms2;
}

double Clock_diffMillis(Clock c1, Clock c2) {
    return _getMillis(&c1.time, &c2.time);
}


/* Clock implementation for Windows */
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)

// source: https://stackoverflow.com/a/5404467

LARGE_INTEGER getFILETIMEoffset() {
    SYSTEMTIME s;
    FILETIME f;
    LARGE_INTEGER t;

    s.wYear = 1970;
    s.wMonth = 1;
    s.wDay = 1;
    s.wHour = 0;
    s.wMinute = 0;
    s.wSecond = 0;
    s.wMilliseconds = 0;
    SystemTimeToFileTime(&s, &f);
    t.QuadPart = f.dwHighDateTime;
    t.QuadPart <<= 32;
    t.QuadPart |= f.dwLowDateTime;
    return (t);
}

int clock_gettime(int X, struct timespec *tv) {
    LARGE_INTEGER           t;
    FILETIME            f;
    double                  microseconds;
    static LARGE_INTEGER    offset;
    static double           frequencyToMicroseconds;
    static int              initialized = 0;
    static BOOL             usePerformanceCounter = 0;

    if (!initialized) {
        LARGE_INTEGER performanceFrequency;
        initialized = 1;
        usePerformanceCounter = QueryPerformanceFrequency(&performanceFrequency);
        if (usePerformanceCounter) {
            QueryPerformanceCounter(&offset);
            frequencyToMicroseconds = (double)performanceFrequency.QuadPart / 1000000.;
        } else {
            offset = getFILETIMEoffset();
            frequencyToMicroseconds = 10.;
        }
    }
    if (usePerformanceCounter) QueryPerformanceCounter(&t);
    else {
        GetSystemTimeAsFileTime(&f);
        t.QuadPart = f.dwHighDateTime;
        t.QuadPart <<= 32;
        t.QuadPart |= f.dwLowDateTime;
    }

    t.QuadPart -= offset.QuadPart;
    microseconds = (double)t.QuadPart / frequencyToMicroseconds;
    t.QuadPart = microseconds;
    tv->tv_sec = t.QuadPart / 1000000;
    tv->tv_nsec = (t.QuadPart % 1000000) * 1000;  // to nanoseconds
    return (0);
}

#endif