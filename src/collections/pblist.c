/** @file
    @brief Generic list implementation
*/

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <progbase/collections/pbenumerator.h>
#include <progbase/collections/pblist.h>

/* array.h */

/**
    @struct Array
    @brief array of generic items
*/
typedef struct Array Array;

/**
    helper type for list methods implementation 
*/
struct Array {
    size_t itemSize;
    char * items;
    size_t length;
};

void Array_copy(
    Array sourceArray,
    int sourceIndex,
    Array destinationArray,
    int destinationIndex,
    int length
);

/* pblist.h */

#define empty(MEM, SIZE) memset(MEM, 0, SIZE)
#define throw(MSG) { assert(0 && MSG); fprintf(stderr, MSG); }

struct PbList {
    int capacity;
    int size;
    void ** items;    /**< holds a pointer to items array */
};

static const int initialCapacity = 4;

static void __ensureCapacity(PbList * self, int min);

PbList * PbList_new(void) {
	PbList * self = malloc(sizeof(struct PbList));
	self->capacity = initialCapacity;
	self->size = 0;
	self->items = malloc(0);
    __ensureCapacity(self, 0);
    return self;
}

void PbList_free(PbList ** selfPtr) {
	if (selfPtr == NULL) throw("Null pointer on free()");
    PbList * self = *selfPtr;
    free(self->items);
    free(self);
    *selfPtr = NULL;
}

void * PbList_get(PbList * self, int index) {
    if (index < 0 || index >= self->size) throw("Index out of bounds");
    return self->items[index];
}
void PbList_set(PbList * self, int index, void * ref) {
    if (ref == NULL) throw("NULL reference");
    if (index < 0 || index >= self->size) throw("Index out of bounds");
    self->items[index] = ref;
}
void PbList_add(PbList * self, void * ref) {
    if (ref == NULL) throw("NULL reference");
    if (self->size <= self->capacity) {
        __ensureCapacity(self, self->size + 1);
    }
    PbList_set(self, self->size++, ref);
}
void PbList_insert(PbList * self, int index, void * ref) {
    if (ref == NULL) throw("NULL reference");
    if (index > self->size) throw("Argument out of range");
    if (self->size >= self->capacity - 1) {
        __ensureCapacity(self, self->size + 1);
    }
    if (index < self->size) {
        Array items = {
            .itemSize = sizeof(void *),
            .items = (char *)self->items,
            .length = self->size
        };
        Array_copy(items, index, items, index + 1, self->size - index);
    }
    PbList_set(self, index, ref);
    self->size++;
}

int PbList_indexOf(PbList * self, void * ref) {
    if (ref == NULL) throw("NULL reference");
    for (int i = 0; i < self->size; i++) {
        if (self->items[i] == ref) {
            return i;
        }
    }
    return -1;
}

bool PbList_contains(PbList * self, void * ref) {
    if (ref == NULL) throw("NULL reference");
    return PbList_indexOf(self, ref) >= 0;
}

bool PbList_remove(PbList * self, void * ref) {
    if (ref == NULL) throw("NULL reference");
    int index = PbList_indexOf(self, ref);
    if (index >= 0) {
        PbList_removeAt(self, index);
        return true;
    }
    return false;
}

void PbList_removeAt(PbList * self, int index) {
    if (index < 0 || index >= self->size) throw("Index out of bounds");
    self->size--;
    if (index < self->size) {
        Array items = {
            .itemSize = sizeof(void *),
            .items = (char *)self->items,
            .length = self->size
        };
        Array_copy(items, index + 1, items, index, self->size - index);
    }
}
bool PbList_isEmpty(PbList * self) {
    return self->size == 0;
}
int  PbList_count(PbList * self) {
    return self->size;
}
void PbList_copyTo(PbList * self, void * array, int arrayIndex) {
    if (arrayIndex < 0 || arrayIndex >= self->size) throw("Index out of bounds");
    memcpy(
        array, 
        self->items[arrayIndex], 
        (self->size - arrayIndex) * sizeof(void *));
}

void PbList_clear(PbList * self) {
    if (self->size > 0) {
        self->size = 0;
    }
}

static void __ensureCapacity(PbList * self, int min) {
    if (self->size <= min) {
        int newCapacity = self->size == 0 ? initialCapacity : self->size * 2;
        if (newCapacity < min) newCapacity = min;
        self->capacity = newCapacity;
        size_t newSize = sizeof(void *) * self->capacity;
        self->items = realloc(self->items, newSize);
    }
}
	
/* array.c */

void Array_copy(
    Array sourceArray,
    int sourceIndex,
    Array destinationArray,
    int destinationIndex,
    int length
) {
    // @todo add checks
    size_t itemSize = sourceArray.itemSize;
    size_t copySize = itemSize * length;
    char buffer[copySize];
    memcpy(buffer, sourceArray.items + (sourceIndex) * itemSize, copySize);
    memcpy(destinationArray.items + (destinationIndex) * itemSize, buffer, copySize);
}

/* list PbEnumerator */

struct __PbEnumerator {
    PbList * list;
    int index;
};

static PbEnumerator * PbEnumerator_new(PbList * list) {
    PbEnumerator * self = malloc(sizeof(PbEnumerator));
    self->list = list;
    PbEnumerator_reset(self);
    return self;
}

PbEnumerator * PbList_getNewPbEnumerator(PbList * self) {
    return PbEnumerator_new(self);
}

void PbEnumerator_free(PbEnumerator * self) {
    free(self);
}

void * PbEnumerator_current(PbEnumerator * self) {
    if (self->index < 0) throw("PbEnumerator in initial state");
    return PbList_get(self->list, self->index);
}

bool PbEnumerator_moveNext(PbEnumerator * self) {
    if (self->index < PbList_count(self->list) - 1) {
        self->index++;
        return true;
    }
    return false;
}

void PbEnumerator_reset(PbEnumerator * self) {
    self->index = -1;
}
