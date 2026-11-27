#ifndef GUARD_INCLUDE_EFFECTS_HEALING_ELEM_LAYOUT_H
#define GUARD_INCLUDE_EFFECTS_HEALING_ELEM_LAYOUT_H

#include "global.h"

struct Elem {
    u32 flags;
    u8 pad4[0xb];
    u8 attr;
    u8 pad10[0x8];
    u16 x;
    u16 y;
    u16 z;
    u8 pad1E[0xa];
    u16 age;
    u16 active;
    u16 dx;
    u16 dy;
    u16 dz;
    u16 pad32; /* confidence=low */
    void *data; /* confidence=low */
    void (*update)(struct Elem *);
};

typedef char ElemSizeCheck[sizeof(struct Elem) == 0x3c ? 1 : -1];

#endif
