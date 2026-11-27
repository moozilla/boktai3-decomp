#ifndef GUARD_INCLUDE_UI_DIALOGUE_TEXT_STATE_H
#define GUARD_INCLUDE_UI_DIALOGUE_TEXT_STATE_H

#include "global.h"

struct TextState {
    u8 x; /* confidence=low */
    u8 y;
    u8 left;
    u8 top;
    u8 width;
    u8 height;
    u8 palette;
    u8 style;
    u8 entered; /* confidence=low */
    u8 mode; /* confidence=low */
    u8 skip;
    u8 busy;
    u8 padC; /* confidence=low */
    u8 choiceCount;
    u8 padE; /* confidence=low */
    u8 fresh;
    u8 pad10; /* confidence=low */
    u8 voice;
    u8 toggle;
    u8 lastWidth;
    u8 pad14; /* confidence=low */
    s8 current;
    u8 instant;
    u8 wrap;
    u32 frame;
    const u8 *pc;
    const u8 *streams[4];
    u8 pad30[0xd8];
    u8 depth;
    u8 styles[7];
};

typedef char TextStateSizeCheck[sizeof(struct TextState) == 0x110 ? 1 : -1];

#endif
