#ifndef GUARD_INCLUDE_UI_BOOKS_BESTIARY_PALETTE_VARIANT_H
#define GUARD_INCLUDE_UI_BOOKS_BESTIARY_PALETTE_VARIANT_H

#include "global.h"

struct BestiaryPaletteVariant {
    u8 pad0[0x1c];
    u32 variant;
    u32 frame;
    u8 state;
    u8 pad25[0x3];
    u8 anim[0x9d4]; /* confidence=low */
    u16 counter; /* confidence=low */
    u8 pad9FE[0x2];
    u16 enabled;
    u8 flag; /* confidence=low */
    u8 padA03[0x1];
};

typedef char BestiaryPaletteVariantSizeCheck[sizeof(struct BestiaryPaletteVariant) == 0xa04 ? 1 : -1];

#endif
