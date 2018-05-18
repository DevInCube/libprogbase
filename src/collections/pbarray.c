/** @file
    @brief Generic array implementation
*/

#include <progbase/collections/pbarray.h>
#include <string.h>

void PbArray_copy(
    PbArray sourcePbArray,
    int sourceIndex,
    PbArray destinationPbArray,
    int destinationIndex,
    int length
) {
    // @todo add checks
    size_t itemSize = sourcePbArray.itemSize;
    size_t copySize = itemSize * length;
    char buffer[copySize];
    memcpy(buffer, sourcePbArray.items + (sourceIndex) * itemSize, copySize);
    memcpy(destinationPbArray.items + (destinationIndex) * itemSize, buffer, copySize);
}