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

/**
    @struct PbList
    @brief defines a generic PbList data type
*/
typedef struct PbList PbList;

/**
    @brief PbList constructor
*/
PbList * PbList_new(void);
/** 
    @brief PbList destructor
*/
void PbList_free(PbList * self);
/**
    @brief PbList items getter
    @param index - index of element
    @returns an item reference at index
*/
void * PbList_at(PbList * self, int index);
/**
    @brief PbList items setter
    @param index - index of element
    @param ref - reference to memory from where to read new value
*/
void PbList_set(PbList * self, int index, void * ref);
/**
    @brief adds new item to the end of PbList
    @param ref - reference to memory from where to read new value
*/
void PbList_add(PbList * self, void * ref);
/**
    @brief inserts new item into PbList at specified position
    @param index - index of element to insert before
    @param ref - reference to memory from where to read new value
*/
void PbList_insert(PbList * self, int index, void * ref);
/**
    @brief remove item from list by value
    @param ref - reference to memory from where to read compared value
    @returns true if success removal
*/
bool PbList_remove(PbList * self, void * ref);
/**
    @brief remove item at index
    @param index - index of element to remove
*/
void PbList_removeAt(PbList * self, int index);
/**
    @brief determines if list contains any elements
*/
bool PbList_isEmpty(PbList * self);
/** 
    @returns a number of items in list
*/
int  PbList_count(PbList * self);
/**
    @param ref - points to memory  with value to find
    @returns index of matched value or -1 otherwise
*/
int  PbList_indexOf(PbList * self, void * ref);
/**
    @brief some brief

    Detailed description
*/
bool PbList_contains(PbList * self, void * ref);
/**
    @brief copy all elements starting from arrayIndex into array memory
    @param array - pointer to destination memory
    @param arrayIndex - index of PbList element to start copy from
*/
void PbList_copyTo(PbList * self, void * array, int arrayIndex);
/**
    @brief clear all list elements
*/
void PbList_clear(PbList * self);

//void PbList_clearFree(PbList * self, );
/**
    @brief returns new PbList PbEnumerator to iterate over PbList elements
*/
PbEnumerator * PbList_getNewPbEnumerator(PbList * self);

#ifdef __cplusplus
}
#endif
