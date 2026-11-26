#ifndef GUARD_PALETTE_RESOURCE_H
#define GUARD_PALETTE_RESOURCE_H

#include "global.h"

/* Palette resources carry a 20-byte header followed by sixteen RGB15 banks. */
struct PaletteResource {
    u8 unknownHeader[0x14];
    u16 banks[16][16];
};

#endif
