#pragma once 

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __Enumerator Enumerator;

void Enumerator_free(Enumerator * self);

void * Enumerator_current(Enumerator * self);
bool Enumerator_moveNext(Enumerator * self);
void Enumerator_reset(Enumerator * self);

#ifdef __cplusplus
}
#endif
