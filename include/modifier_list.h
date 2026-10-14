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
struct ModifierGraphicsResource {
    u32 words[8];
};
struct ModifierDisplayObject {
    u8 unknown0[8];
    u32 flags;
    u8 unknownC[0xe];
    u8 attribute1A;
    u8 unknown1B[5];
    s16 x, y, z;
    u8 unknown26[0x14];
    u16 tile;
    u8 unknown3C[0xc];
    void *tileData;
    u8 unknown4C[0x14];
};
/* Known prefix only: later fields include the 3D preview and more menu state. */
struct ModifierMenu {
    u8 task[0x18];
    s32 state, mode;
    s8 category, index;
    u8 unknown22[0x12];
    void *backgroundResource;
    void *tilemap;
    u8 unknown3C[0x2c];
    struct ModifierSelection current, pending;
    void *script;
    u8 unknownBC[0x55];
    u8 specialHeader;
    u8 unknown112[2];
    u8 changed;
    u8 unknown115[3];
    u8 glyphData[0x24];
    struct ModifierGraphicsResource graphicsA, graphicsB;
    struct ModifierDisplayObject displays[18];
    u8 unknown83C[0x6c0];
    u32 previewFlags;
};
struct ModifierItemInfo {
    u8 id, kind;
    u16 value2, value4, value6, value8;
};
s32 sub_0811E37C(struct ModifierRecord *, s32);
#endif
