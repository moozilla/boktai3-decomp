#ifndef GUARD_CHARACTER_PREVIEW_MODEL_H
#define GUARD_CHARACTER_PREVIEW_MODEL_H
#include "global.h"
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
/* Five display parts share storage with preview metadata at 0xD2. */
struct CharacterPreviewModel {
    union {
        struct ModifierDisplayObject parts[5];
        struct {
            u8 unknown0[0xD2];
            u16 kind;
        } metadata;
    } view;
    u8 unknown1E0[0x30];
    u16 activePalette[16], normalPalette[16];
};
#endif
