// MATCHING_CONCESSION: register-pinning
#include "modifier_list.h"
#define ModifierMenu_GetHeaderFrame sub_08040A74
s32 ModifierMenu_GetHeaderFrame(struct ModifierMenu *menu)
{
    struct ModifierMenu *base = menu;
    s32 frame = (s32)menu;
    asm("" : "+r"(base));
    switch (base->mode) {
        case 0:
            frame = base->choice.cursor.category + 19;
            break;
        case 1:
            frame = base->choice.cursor.category + 23;
            break;
        case 2:
            frame = base->choice.cursor.category + 27;
            break;
    }
    asm("" : : "r"(base));
    /* The original preserves the input register for an unrecognized mode. */
    return frame;
}
