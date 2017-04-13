/** @file
    @brief Generic list implementation
*/

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <progbase/list.h>

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

/* list.h */

#define empty(MEM, SIZE) memset(MEM, 0, SIZE)
#define throw(MSG) assert(0 && MSG);

struct List {
    int capacity;
    int size;
    void ** items;    /**< holds a pointer to items array */
};

static const int initialCapacity = 4;

static void __ensureCapacity(List * self, int min);

List * List_new(void) {
	List * self = malloc(sizeof(struct List));
	self->capacity = initialCapacity;
	self->size = 0;
	self->items = malloc(0);
    __ensureCapacity(self, 0);
    return self;
}

void List_free(List ** selfPtr) {
	if (selfPtr == NULL) throw("Null pointer on free()");
    List * self = *selfPtr;
    free(self->items);
    free(self);
    *selfPtr = NULL;
}

void * List_get(List * self, int index) {
    if (index < 0 || index >= self->size) throw("Index out of bounds");
    return self->items[index];
}
void List_set(List * self, int index, void * ref) {
    if (ref == NULL) throw("NULL reference");
    if (index < 0 || index >= self->size) throw("Index out of bounds");
    self->items[index] = ref;
}
void List_add(List * self, void * ref) {
    if (ref == NULL) throw("NULL reference");
    if (self->size <= self->capacity) {
        __ensureCapacity(self, self->size + 1);
    }
    List_set(self, self->size++, ref);
}
void List_insert(List * self, int index, void * ref) {
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
    List_set(self, index, ref);
    self->size++;
}

int List_indexOf(List * self, void * ref) {
    if (ref == NULL) throw("NULL reference");
    for (int i = 0; i < self->size; i++) {
        if (self->items[i] == ref) {
            return i;
        }
    }
    return -1;
}

bool List_contains(List * self, void * ref) {
    if (ref == NULL) throw("NULL reference");
    return List_indexOf(self, ref) >= 0;
}

bool List_remove(List * self, void * ref) {
    if (ref == NULL) throw("NULL reference");
    int index = List_indexOf(self, ref);
    if (index >= 0) {
        List_removeAt(self, index);
        return true;
    }
    return false;
}

void List_removeAt(List * self, int index) {
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
bool List_isEmpty(List * self) {
    return self->size == 0;
}
int  List_count(List * self) {
    return self->size;
}
void List_copyTo(List * self, void * array, int arrayIndex) {
    if (arrayIndex < 0 || arrayIndex >= self->size) throw("Index out of bounds");
    memcpy(
        array, 
        self->items[arrayIndex], 
        (self->size - arrayIndex) * sizeof(void *));
}

void List_clear(List * self) {
    if (self->size > 0) {
        self->size = 0;
    }
}

static void __ensureCapacity(List * self, int min) {
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
