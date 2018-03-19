/** @file
    @brief EventSystem Interface
*/

#pragma once

#ifndef __INT_MAX__
    #define __INT_MAX__ 2147483647
#endif

#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
	@typedef DestructorFunction
	@brief a pointer type for functions that can free generic pointed data
*/
typedef void (*DestructorFunction)(void * data);

/**
	@typedef ESObject
	@brief an outer reference counting object to hold reference to some data 
	along with it's destructor function
*/
typedef struct ESObject ESObject;

/**
	@typedef Event
*/
typedef struct Event Event;
/**
	@typedef EventHandler
	@brief a container for event handler information
*/
typedef struct EventHandler EventHandler;

/** 
	@brief a new ESObject constructor with parameters
	@param ref - a reference to some data
	@param destructor - data destructor function
*/
ESObject * ESObject_new(void * ref, DestructorFunction destructor);

/*
	@brief get data reference from ESObject
*/
void * ESObject_ref(ESObject * self);

/**
	@brief reference counting ref increase
*/
void ESObject_incref(ESObject * self);

/**
	@brief reference counting ref decrease
	free's object when reference counter becomes 0
*/
void ESObject_decref(ESObject * self);

/**
	@struct Event
	@brief a structure that holds information about an event occured
*/
struct Event {
	EventHandler * sender;  /**< pointer to an event handler that have raised this event */
	int type;  /**< an identifier of event type  */
	ESObject * payload;  /**< pointer to custom event data of type depending on event type */
};

/** 
	@brief a new Event constructor with parameters
	@param sender - who generated this event
	@param type - event type
	@param payload - pointer to specific event data object
*/
Event * Event_new(
	EventHandler * sender, 
	int type, 
	ESObject * payload);

/**
	@typedef EventHandlerFunction
	@brief a type for callback functions to call on event handlers on event occurance
	@param self - event handler data pointer
	@param event - newly raised event
*/
typedef void (*EventHandlerFunction)(EventHandler * self, Event * event);

/**
	@brief a structure that holds infomation about system event handlers
*/
struct EventHandler {
	ESObject * state;  /**< a pointer to an event handler data */
	EventHandlerFunction handler;  /**< a pointer to function that will call on data events handle */
};

/**
	@param state - an event handler state object
	@param eventHandler - an EventHandlerFunction callback to handle events for data event handler
*/
EventHandler * EventHandler_new(
	ESObject * state, 
	EventHandlerFunction eventHandler);

/* public EventSystem API */

/**
	@brief initialize all EventSystem data
*/
void EventSystem_init(void);
/**
	@brief cleanup all EventSystem data
*/
void EventSystem_cleanup(void);
/**
	@brief start infinite event loop
*/
void EventSystem_loop(void);
/**
	@brief stops infinite event loop
*/
void EventSystem_exit(void);
/**
	@brief add new EventSystem event handler
*/
void EventSystem_addHandler(EventHandler * handler);
/**
	@brief remove EventSystem event handler by it's data pointer' 
	from event system handlers list and destoys it
*/
void EventSystem_removeHandler(EventHandler * handler);
/**
	@brief add new event to EventSystem to handle by event handlers
*/
void EventSystem_emit(Event * event);

/**
	@brief some base events type ids
*/
typedef enum { 
	StartEventTypeId = INT_MIN,  /**< the first event generated before event loop start */
	UpdateEventTypeId,  /**< event is generated in every iteration of event loop */
	ExitEventTypeId  /**< event to stop event loop */
} BaseEventTypes;

/**
    @brief get elapsed milliseconds value from UpdateEvent
*/
double UpdateEvent_elapsedMillis(Event * event);

#ifdef __cplusplus
}
#endif
