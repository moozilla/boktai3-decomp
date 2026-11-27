#ifndef GUARD_INCLUDE_UI_DIALOGUE_TEXT_WINDOW_H
#define GUARD_INCLUDE_UI_DIALOGUE_TEXT_WINDOW_H

#include "global.h"
#include "ui/dialogue/text_state.h"

struct TextWindow {
    s32 id;
    u8 x;
    u8 y;
    u8 width;
    u8 height;
    u32 enabled;
    u16 clock;
    u16 index;
    s16 symbol;
    u16 pad12; /* confidence=low */
    const u8 *script;
    u16 sequenceIndex;
    u16 sequenceCount;
    u16 sequence[32];
    void *callback;
    struct TextState text;
    u8 pad170[0x54];
    struct TextWindow *next;
    struct TextWindow *previous;
};

typedef char TextWindowSizeCheck[sizeof(struct TextWindow) == 0x1cc ? 1 : -1];

#endif
