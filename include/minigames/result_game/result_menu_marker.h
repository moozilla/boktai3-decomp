#ifndef GUARD_INCLUDE_MINIGAMES_RESULT_GAME_RESULT_MENU_MARKER_H
#define GUARD_INCLUDE_MINIGAMES_RESULT_GAME_RESULT_MENU_MARKER_H

#include "global.h"

struct ResultMenuMarkerGraphic {
    u32 pad[2]; /* confidence=low */
    u32 flags;
    u8 padC[0xe];
    u8 layer; /* confidence=low */
    u8 pad1B[0x5];
    s16 x; /* confidence=low */
    s16 y; /* confidence=low */
    s16 z; /* confidence=low */
    u8 tail[0x3a]; /* confidence=low */
};

typedef char ResultMenuMarkerGraphicSizeCheck[sizeof(struct ResultMenuMarkerGraphic) == 0x60 ? 1 : -1];

#endif
