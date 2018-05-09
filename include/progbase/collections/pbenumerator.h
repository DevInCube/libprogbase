#pragma once 

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __PbEnumerator PbEnumerator;

void PbEnumerator_free(PbEnumerator * self);

void * PbEnumerator_current(PbEnumerator * self);
bool PbEnumerator_moveNext(PbEnumerator * self);
void PbEnumerator_reset(PbEnumerator * self);

#ifdef __cplusplus
}
#endif
