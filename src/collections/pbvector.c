/** @file
    @brief Generic list implementation
*/

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pbenumerator.h"
#include "pbvector.h"
#include "pbarray.h"

#define empty(MEM, SIZE) memset(MEM, 0, SIZE)
#define throw(MSG) { fprintf(stderr, MSG); assert(0 && MSG); return; }
#define throw_return(MSG, RETURN_VALUE) { fprintf(stderr, MSG); assert(0 && MSG); return RETURN_VALUE; }

struct PbVector {
    int capacity;
    int size;
    size_t itemSize;
    void ** items;    /**< holds a pointer to items array */
};

static const int initialCapacity = 4;

static void __ensureCapacity(PbVector * self, int min);

PbVector * PbVector_new(size_t itemSize) {
	PbVector * self = malloc(sizeof(struct PbVector));
	self->capacity = initialCapacity;
	self->size = 0;
    self->itemSize = itemSize;
	self->items = malloc(0);
    __ensureCapacity(self, 0);
    return self;
}

void PbVector_free(PbVector * self) {
    free(self->items);
    free(self);
}

void PbVector_at(PbVector * self, int index, void * ref) {
    if (index < 0 || index >= self->size) throw("Index out of bounds");
    ref = &self->items[index];
}
void PbVector_set(PbVector * self, int index, void * ref) {
    if (ref == NULL) throw("NULL reference");
    if (index < 0 || index >= self->size) throw("Index out of bounds");
    self->items[index] = ref;
}
void PbVector_add(PbVector * self, void * ref) {
    if (ref == NULL) throw("NULL reference");
    if (self->size <= self->capacity) {
        __ensureCapacity(self, self->size + 1);
    }
    PbVector_set(self, self->size++, ref);
}
void PbVector_insert(PbVector * self, int index, void * ref) {
    if (ref == NULL) throw("NULL reference");
    if (index > self->size) throw("Argument out of range");
    if (self->size >= self->capacity - 1) {
        __ensureCapacity(self, self->size + 1);
    }
    if (index < self->size) {
        PbArray items = {
            .itemSize = sizeof(void *),
            .items = (char *)self->items,
            .length = self->size
        };
        PbArray_copy(items, index, items, index + 1, self->size - index);
    }
    PbVector_set(self, index, ref);
    self->size++;
}

int PbVector_indexOf(PbVector * self, void * ref) {
    if (ref == NULL) throw_return("NULL reference", -1);
    for (int i = 0; i < self->size; i++) {
        if (self->items[i] == ref) {
            return i;
        }
    }
    return -1;
}

bool PbVector_contains(PbVector * self, void * ref) {
    if (ref == NULL) throw_return("NULL reference", false);
    return PbVector_indexOf(self, ref) >= 0;
}

bool PbVector_remove(PbVector * self, void * ref) {
    if (ref == NULL) throw_return("NULL reference", false);
    int index = PbVector_indexOf(self, ref);
    if (index >= 0) {
        PbVector_removeAt(self, index);
        return true;
    }
    return false;
}

void PbVector_removeAt(PbVector * self, int index) {
    if (index < 0 || index >= self->size) throw("Index out of bounds");
    self->size--;
    if (index < self->size) {
        PbArray items = {
            .itemSize = sizeof(void *),
            .items = (char *)self->items,
            .length = self->size
        };
        PbArray_copy(items, index + 1, items, index, self->size - index);
    }
}
bool PbVector_isEmpty(PbVector * self) {
    return self->size == 0;
}
int  PbVector_count(PbVector * self) {
    return self->size;
}
void PbVector_copyTo(PbVector * self, void * array, int arrayIndex) {
    if (arrayIndex < 0 || arrayIndex >= self->size) throw("Index out of bounds");
    memcpy(
        array, 
        self->items[arrayIndex], 
        (self->size - arrayIndex) * sizeof(void *));
}

void PbVector_clear(PbVector * self) {
    if (self->size > 0) {
        self->size = 0;
    }
}

void PbVector_forEach(PbVector * self, PbVectorForEachCallback callback, void * context) {
    char * itemMem = malloc(self->itemSize);
    for (int i = 0; i < self->size; i++) {
        PbVector_at(self, i, itemMem);
        callback(itemMem, i, self, context);
    }
    free(itemMem);
}

static void __ensureCapacity(PbVector * self, int min) {
    if (self->size <= min) {
        int newCapacity = self->size == 0 ? initialCapacity : self->size * 2;
        if (newCapacity < min) newCapacity = min;
        self->capacity = newCapacity;
        size_t newSize = sizeof(void *) * self->capacity;
        void * newMem = realloc(self->items, newSize);
        if (newMem == NULL) throw("Realloc error");
        self->items = newMem;
    }
}
