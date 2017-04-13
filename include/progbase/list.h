/** @file
    @brief Generic list abstract data type
*/

#pragma once

#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
    @struct List
    @brief defines a generic List data type
*/
typedef struct List List;
// typedef struct ListPrivate ListPrivate;
// struct List {
//     // implements(Enumerable);
// 	ListPrivate * priv;
// };

/**
    @brief List constructor
*/
List * List_new(void);
/** 
    @brief List destructor
*/
void List_free(List ** self);
/**
    @brief List items getter
    @param index - index of element
    @param ref - reference to memory where to write the value
*/
void * List_get(List * self, int index);
/**
    @brief List items setter
    @param index - index of element
    @param ref - reference to memory from where to read new value
*/
void List_set(List * self, int index, void * ref);
/**
    @brief adds new item to the end of List
    @param ref - reference to memory from where to read new value
*/
void List_add(List * self, void * ref);
/**
    @brief inserts new item into List at specified position
    @param index - index of element to insert before
    @param ref - reference to memory from where to read new value
*/
void List_insert(List * self, int index, void * ref);
/**
    @brief remove item from list by value
    @param ref - reference to memory from where to read compared value
    @returns true if success removal
*/
bool List_remove(List * self, void * ref);
/**
    @brief remove item at index
    @param index - index of element to remove
*/
void List_removeAt(List * self, int index);
/**
    @brief determines if list contains any elements
*/
bool List_isEmpty(List * self);
/** 
    @returns a number of items in list
*/
int  List_count(List * self);
/**
    @param ref - points to memory  with value to find
    @returns index of matched value or -1 otherwise
*/
int  List_indexOf(List * self, void * ref);
/**
    @brief some brief

    Detailed description
*/
bool List_contains(List * self, void * ref);
/**
    @brief copy all elements starting from arrayIndex into array memory
    @param array - pointer to destination memory
    @param arrayIndex - index of List element to start copy from
*/
void List_copyTo(List * self, void * array, int arrayIndex);
/**
    @brief clear all list elements
*/
void List_clear(List * self);

//void List_clearFree(List * self, );
/**
    @brief returns a List Enumerator to iterate over List elements
*/
// Enumerator * List_getEnumerator(List * self);

#ifdef __cplusplus
}
#endif
