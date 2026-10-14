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
struct ModifierSelection {
    s16 values0[5];
    s16 count, capacity;
    s16 valuesE[5];
    u32 mask;
    s16 sourceIds[4];
    s16 selectedId, secondaryId;
};
struct ModifierMenu {
    u8 task[0x18];
    s32 state, mode;
    s8 category, index;
    u8 unknown22[0x46];
    struct ModifierSelection current, pending;
    void *script;
    u8 unknownBC[0x58];
    u8 changed;
};
struct ModifierItemInfo {
    u8 id, kind;
    u16 value2, value4, value6, value8;
};
s32 sub_0811E37C(struct ModifierRecord *, s32);
#endif
