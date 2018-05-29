#include <stdlib.h>
#include <stdio.h>

#include <progbase/collections/pbvector.h>

void printInt(int * item, int index) {
    printf("  [%i] = %i\n", index, *item);
}

int main(void) {
    PbVector * numbers = PbVector_new(sizeof(int));
    PbVector_add(numbers, PbRef(5));
    PbVector_add(numbers, PbRef(-2));
    PbVector_add(numbers, PbRef(13));
    printf("%i items:\n", PbVector_count(numbers));
    PbVector_forEach(numbers, (PbVectorForEachCallback)printInt, NULL);
    PbVector_free(numbers);
    return 0;
}