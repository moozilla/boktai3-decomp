#ifndef GUARD_INCLUDE_UI_BOOKS_PALETTE_TRANSITION_BANKS_H
#define GUARD_INCLUDE_UI_BOOKS_PALETTE_TRANSITION_BANKS_H

#include "global.h"

struct PaletteTransitionBanks {
    u16 source[16][16];
    u16 destination[16][16];
    u16 target[16][16];
    u8 pad600[0x2];
    u8 firstBank;
    u8 extraBank;
};

typedef char PaletteTransitionBanksSizeCheck[sizeof(struct PaletteTransitionBanks) == 0x604 ? 1 : -1];

#endif
