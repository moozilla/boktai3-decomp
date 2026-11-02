#ifndef GUARD_ARMOR_PALETTE_H
#define GUARD_ARMOR_PALETTE_H

#include "global.h"

struct ArmorPaletteController {
    void (*update)(void *);
    void (*finish)(void *);
    u16 palettes[16][16]; /* 0x008 */
    u16 banks[3][2][16];  /* 0x208 */
    u8 unknown2c8[0x408 - 0x2c8];
    u16 saved[4][16]; /* 0x408 */
    u16 timer;        /* 0x488 */
    u8 bank;          /* 0x48a */
    u8 otherBank;     /* 0x48b */
};

struct Armor {
    u8 unknown0[0xee];
    u16 paletteId; /* 0x0ee */
    u8 unknownf0[0xfc - 0xf0];
    void *palette; /* 0x0fc */
    u8 unknown100[0x2e6 - 0x100];
    u16 bankId0; /* 0x2e6 */
    u8 unknown2e8[0x2ec - 0x2e8];
    void *bank0; /* 0x2ec */
    u8 unknown2f0[0x302 - 0x2f0];
    u16 bankId1; /* 0x302 */
    u8 unknown304[0x308 - 0x304];
    void *bank1; /* 0x308 */
    u8 unknown30c[0x31e - 0x30c];
    u16 bankId2; /* 0x31e */
    u8 unknown320[0x324 - 0x320];
    void *bank2; /* 0x324 */
    u8 unknown328[0x33a - 0x328];
    u16 bankId3; /* 0x33a */
    u8 unknown33c[0x340 - 0x33c];
    void *bank3; /* 0x340 */
    u8 unknown344[0x650 - 0x344];
    struct ArmorPaletteController paletteController; /* 0x650 */
    u8 unknownadc[0xb32 - 0xadc];
    u16 flags; /* 0xb32 */
    u8 unknownb34[0xb38 - 0xb34];
    u8 kind; /* 0xb38 */
};

#endif
