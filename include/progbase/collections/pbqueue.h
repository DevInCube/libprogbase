#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
	@struct PbQueue
	@brief a queue of items
*/
typedef struct PbQueue PbQueue;

PbQueue * PbQueue_new(void);
void    PbQueue_free(PbQueue ** selfPtr);

/**
	@brief adds new Event to the PbQueue tail
	@param event - new event
*/
void    PbQueue_enqueue(PbQueue *  self, void * value);

/**
	@brief removes a n Event from the PbQueue head
	@returns Event from head or NULL otherwise
*/
void *  PbQueue_dequeue(PbQueue *  self);

/**
	@returns the number of events in PbQueue
*/
int     PbQueue_size(PbQueue * self);

#ifdef __cplusplus
}
#endif
