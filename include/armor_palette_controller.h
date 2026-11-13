#ifndef GUARD_ARMOR_PALETTE_CONTROLLER_H
#define GUARD_ARMOR_PALETTE_CONTROLLER_H
#include "global.h"

struct ArmorPaletteController {
    void (*update)(struct ArmorPaletteController *);
    void (*finish)(struct ArmorPaletteController *);
    u16 palettes[16][16]; /* 0x008 */
    u16 working[16][16]; /* 0x208 */
    u16 saved[4][16]; /* 0x408 */
    s16 timer;        /* 0x488 */
    u8 bank;          /* 0x48a */
    u8 otherBank;     /* 0x48b */
};

#endif
