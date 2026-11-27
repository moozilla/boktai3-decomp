#ifndef GUARD_INCLUDE_OBJECTS_DECK_CANNONS_SCRIPT_VALUE_ROTATION_LAYOUT_H
#define GUARD_INCLUDE_OBJECTS_DECK_CANNONS_SCRIPT_VALUE_ROTATION_LAYOUT_H

#include "global.h"

struct ScriptValueRotation {
    u8 pad0[0x78];
    s8 startIndex;
    u8 ticks;
    u8 pad7A[0x2];
} __attribute__((aligned(4)));

typedef char ScriptValueRotationSizeCheck[sizeof(struct ScriptValueRotation) == 0x7c ? 1 : -1];

#endif
