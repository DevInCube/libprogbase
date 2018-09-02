/** @file
    @brief Generic array implementation
*/

#include <progbase/collections/pbarray.h>
#include <string.h>
#include <assert.h>

void PbArray_copy(
    PbArray sourceAr,
    int sourceIndex,
    PbArray destAr,
    int destinationIndex,
    int length
) {
    assert(sourceIndex >= 0 && sourceIndex < (int)sourceAr.length);
    assert(destinationIndex >= 0 && destinationIndex < (int)destAr.length);
    if (sourceAr.itemSize != destAr.itemSize) { return; }  // can't copy when items are of different types
    if (sourceIndex + length > (int)sourceAr.length) {
        length = sourceAr.length - sourceIndex;  // trim copy length
    }
    if (destinationIndex + length > (int)destAr.length) {
        length = destAr.length - destinationIndex;  // trim copy length
    }
    size_t itemSize = sourceAr.itemSize;
    size_t copySize = itemSize * length;
    char * buffer = malloc(copySize * sizeof(char));
    memcpy(buffer, sourceAr.items + (sourceIndex) * itemSize, copySize);
    memcpy(destAr.items + (destinationIndex) * itemSize, buffer, copySize);
    free(buffer);
}
