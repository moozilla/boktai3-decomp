#ifndef GUARD_INCLUDE_LINK_SELECTION_LINK_SELECTION_PROMPT_H
#define GUARD_INCLUDE_LINK_SELECTION_LINK_SELECTION_PROMPT_H

#include "global.h"

struct LinkSelectionPrompt {
    u8 pad0[0x1a];
    u16 count;
    u8 pad1C[0x14];
    u32 timer;
    u8 pad34[0xa];
    u16 flags; /* confidence=low */
    u16 *input;
    u32 field44; /* confidence=low */
    u32 state;
    u8 pad4C[0x6c];
    s32 window;
    u8 padBC[0x1a0];
    const u8 *script;
    u32 field260; /* confidence=low */
    u32 field264; /* confidence=low */
};

typedef char LinkSelectionPromptSizeCheck[sizeof(struct LinkSelectionPrompt) == 0x268 ? 1 : -1];

#endif
