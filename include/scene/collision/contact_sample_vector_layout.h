#ifndef GUARD_INCLUDE_RECOVERED_CONTACT_SAMPLE_VECTOR_LAYOUT_H
#define GUARD_INCLUDE_RECOVERED_CONTACT_SAMPLE_VECTOR_LAYOUT_H

#include "global.h"

struct ContactSampleVector {
    s32 x : 16; /* confidence=low */
    s32 y : 16; /* confidence=low */
    s32 z : 16; /* confidence=low */
    s32 pad : 16; /* confidence=low */
};

typedef char ContactSampleVectorSizeCheck[sizeof(struct ContactSampleVector) == 0x8 ? 1 : -1];

#endif
