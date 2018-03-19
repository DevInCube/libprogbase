#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
	@struct Queue
	@brief a queue of items
*/
typedef struct Queue Queue;

Queue * Queue_new(void);
void    Queue_free(Queue ** selfPtr);

/**
	@brief adds new Event to the Queue tail
	@param event - new event
*/
void    Queue_enqueue(Queue *  self, void * value);

/**
	@brief removes a n Event from the Queue head
	@returns Event from head or NULL otherwise
*/
void *  Queue_dequeue(Queue *  self);

/**
	@returns the number of events in Queue
*/
int     Queue_size(Queue * self);

#ifdef __cplusplus
}
#endif
