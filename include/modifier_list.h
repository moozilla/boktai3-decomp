#ifndef GUARD_MODIFIER_LIST_H
#define GUARD_MODIFIER_LIST_H
/* Tentative: selected source records used by the adjacent stat modifier routines. */
#include "engine.h"
struct ModifierRecord {
    s32 order : 8;
    u32 kind : 8;
    u32 amount : 16;
    u8 extra;
    u8 unknown5[3];
};
struct ModifierActor {
    u8 unknown0[0x24];
    s16 selectedId;
};
struct ModifierSelection {
    u8 unknown0[0xa];
    s16 count;
    u8 unknownC[12];
    u32 mask;
};
s32 sub_0811E37C(struct ModifierRecord *, s32);
#endif
