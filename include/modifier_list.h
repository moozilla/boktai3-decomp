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
    u16 *palette; /* 32-byte graphic entry; palette interpretation is tentative. */
    u8 unknown4C[0x14];
};
struct ModifierCursor {
    s8 category, index;
    u8 unknown2[2];
};
union ModifierCursorWord {
    struct ModifierCursor cursor;
    u32 word;
};
/* Observed layout through 0x1088, the size allocated by 08042158.
 * Preview resource/model storage remains opaque within unknown83C. */
struct ModifierMenu {
    u8 task[0x18];
    s32 state, mode;
    union ModifierCursorWord choice;
    u32 savedChoices[3];
    u32 stateFrame;
    void *backgroundResource;
    void *tilemap;
    u8 unknown3C[0x28];
    u32 dirtyFlags;
    struct ModifierSelection current, pending;
    void *script;
    const u8 *textSources[13];
    s32 mainWindow, detailWindow;
    s32 listWindows[6];
    u8 inputDelay;
    u8 specialHeader;
    u8 unknown112[2];
    u8 changed;
    u8 errorFlash;
    u8 unknown116, colorCycle;
    u16 palette[16]; /* copied as 16 halfwords by 08217DB4; interpretation tentative. */
    u8 modeRepeat, indexRepeat, categoryRepeat, unknown13B;
    struct ModifierGraphicsResource graphicsA, graphicsB;
    struct ModifierDisplayObject displays[18];
    u8 unknown83C[0x6c0];
    u32 previewFlags;
    u8 unknownF00[0x186];
    u8 previewActorId;
    u8 unknown1087;
};
/* Prefix observed through the menu cancellation check; gameplay identity tentative. */
struct ModifierMenuActor {
    u8 unknown0[0x18];
    s16 id;
    u8 unknown1A[2];
    u8 active;
};
struct ModifierPreviewOrigin {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};
struct ModifierItemInfo {
    u8 id, kind;
    u16 value2, value4, value6, value8;
};
s32 sub_0811E37C(struct ModifierRecord *, s32);
#endif
