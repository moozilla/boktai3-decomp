#ifndef GUARD_INCLUDE_RECOVERED_CONTACT_REACTION_VECTOR_LAYOUT_H
#define GUARD_INCLUDE_RECOVERED_CONTACT_REACTION_VECTOR_LAYOUT_H

#include "global.h"

struct ContactReactionVector {
    s16 x; /* confidence=low */
    s16 y; /* confidence=low */
    s16 z; /* confidence=low */
    s16 pad; /* confidence=low */
} __attribute__((aligned(4)));

typedef char ContactReactionVectorSizeCheck[sizeof(struct ContactReactionVector) == 0x8 ? 1 : -1];

#endif
