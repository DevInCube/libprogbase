#include <stdlib.h>
#include <stdio.h>

#include <progbase/collections/pbvector.h>

void printInt(int * item) {
    printf("%i, ", *item);
}

int main(void) {
    PbVector * numbers = PbVector_new(sizeof(int));
    PbVector_add(numbers, &(int){5});
    PbVector_add(numbers, &(int){-2});
    PbVector_add(numbers, &(int){13});
    printf("%i) ", PbVector_count(numbers));
    PbVector_forEach(numbers, (PbVectorForEachCallback)printInt, NULL);
    PbVector_free(numbers);
    return 0;
}