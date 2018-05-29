/** @file
    @brief Generic list abstract data type
*/

#pragma once

#include <stdlib.h>
#include <stdbool.h>

#include "pbenumerator.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PbValue PbValue;
struct PbValue {
    void * ref;
    size_t size;
};

/**
    @struct PbVector
    @brief defines a generic PbVector data type
*/
typedef struct PbVector PbVector;

/**
    @brief PbVector constructor
*/
PbVector * PbVector_new(size_t itemSize);
/** 
    @brief PbVector destructor
*/
void PbVector_free(PbVector * self);
/**
    @brief PbVector items getter
    @param index - index of element
    @returns an item reference at index
*/
void PbVector_at(PbVector * self, int index, PbValue ref);
/**
    @brief PbVector items setter
    @param index - index of element
    @param ref - reference to memory from where to read new value
*/
void PbVector_set(PbVector * self, int index, PbValue ref);
/**
    @brief adds new item to the end of PbVector
    @param ref - reference to memory from where to read new value
*/
void PbVector_add(PbVector * self, PbValue ref);
/**
    @brief inserts new item into PbVector at specified position
    @param index - index of element to insert before
    @param ref - reference to memory from where to read new value
*/
void PbVector_insert(PbVector * self, int index, PbValue ref);
/**
    @brief remove item from list by value
    @param ref - reference to memory from where to read compared value
    @returns true if success removal
*/
bool PbVector_remove(PbVector * self, void * ref);
/**
    @brief remove item at index
    @param index - index of element to remove
*/
void PbVector_removeAt(PbVector * self, int index);
/**
    @brief determines if list contains any elements
*/
bool PbVector_isEmpty(PbVector * self);
/** 
    @returns a number of items in list
*/
int  PbVector_count(PbVector * self);
/**
    @param ref - points to memory  with value to find
    @returns index of matched value or -1 otherwise
*/
int  PbVector_indexOf(PbVector * self, void * ref);
/**
    @brief some brief

    Detailed description
*/
bool PbVector_contains(PbVector * self, void * ref);
/**
    @brief copy all elements starting from arrayIndex into array memory
    @param array - pointer to destination memory
    @param arrayIndex - index of PbVector element to start copy from
*/
void PbVector_copyTo(PbVector * self, void * array, int arrayIndex);
/**
    @brief clear all list elements
*/
void PbVector_clear(PbVector * self);

/**
 * @brief Function to execute for each element of list in PbVector_forEach() call.
 * @param currentValue - the value of the current element being processed in the list.
 * @param index - the index of the current element being processed in the list.
 * @param list - the list that PbVector_forEach() is being applied to
 * @param context - the custom user data pointer, provided into PbVector_forEach() call
 */
typedef void (*PbVectorForEachCallback)(void * currentValue, int index, PbVector * list, void * context);

/**
 * @brief Executes a provided callback function once for each list element.
 * @param callback - a callback function to 
 */
void PbVector_forEach(PbVector * self, PbVectorForEachCallback callback, void * context);

// extensions
#define PBVALUE(VALUE) ((PbValue){(&(__typeof__(VALUE)){VALUE}), sizeof(VALUE)})

#ifdef __cplusplus
}
#endif
