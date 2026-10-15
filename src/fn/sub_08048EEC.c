// MATCHING_CONCESSION: register-pinning
#include "tether_actor.h"
s32 DisplayAnimation_SelectPaletteBank(struct DisplayAnimation *, u32);
s32 DisplayAnimation_SetTileSource(struct DisplayAnimation *, u32);
void TetherActor_SelectPalette(struct TetherActor *input, u32 index)
{
    struct TetherActor *actor = input;
    register struct DisplayAnimation *display asm("r4") = &actor->display.animation;
    if (display->kind == 2)
        DisplayAnimation_SelectPaletteBank(display, 408);
    else {
        const u16 **sourceField;
        const u16 *source;
        struct DisplayAnimation *savedDisplay;
        u16 *buffer;
        u16 *destination;
        s32 remaining;
        DisplayAnimation_SelectPaletteBank(display, index + 0x283);
        sourceField = &actor->display.colors.palette;
        savedDisplay = display;
        buffer = actor->palette;
        source = *sourceField;
        destination = buffer;
        remaining = 15;
        do {
            *destination = *source;
            ++source;
            ++destination;
            --remaining;
        } while (remaining >= 0);
        DisplayAnimation_SetTileSource(savedDisplay, (u32)buffer);
    }
}
