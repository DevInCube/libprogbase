/** @file
    @brief Generic array data type
*/

#pragma once 

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
    @struct PbArray
    @brief array of generic items
*/
typedef struct PbArray PbArray;

struct PbArray {
    size_t itemSize;
    char * items;
    size_t length;
};

void PbArray_copy(
    PbArray sourcePbArray,
    int sourceIndex,
    PbArray destinationPbArray,
    int destinationIndex,
    int length
);

#ifdef __cplusplus
}
#endif
