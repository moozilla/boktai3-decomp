#ifndef GUARD_INCLUDE_BOSSES_VANARGAND_SCREEN_PALETTE_TRANSITION_LAYOUT_H
#define GUARD_INCLUDE_BOSSES_VANARGAND_SCREEN_PALETTE_TRANSITION_LAYOUT_H

#include "global.h"

struct ScreenPaletteTransition {
    void *resource; /* confidence=low */
    u8 *palettes; /* confidence=low */
    void *coordinates; /* confidence=low */
    u8 offset[8]; /* confidence=low */
    u8 state; /* confidence=low */
    u8 enabled; /* confidence=low */
    u16 timer; /* confidence=low */
};

typedef char ScreenPaletteTransitionSizeCheck[sizeof(struct ScreenPaletteTransition) == 0x18 ? 1 : -1];

#endif
