#include "global.h"
#include "render.h"

struct AxePaletteTransition {
    u16 colors[16];
    u16 remaining, destinationBank;
    s16 red[16], green[16], blue[16];
    s16 deltaRed[16], deltaGreen[16], deltaBlue[16];
};
struct AxePaletteContext {
    u8 unk_00[0x624];
    struct AxePaletteTransition transition;
    u8 unk_708[0xE4C - 0x708];
    u32 flags;
};
s32 Div(s32, s32);

// confidence=low: fades a palette bank toward another, filling per-channel deltas over duration frames
void sub_080FB620(void *object, s32 destinationBank, s32 sourceBank, s32 duration)
{
    struct AxePaletteContext *context = object;
    struct AxePaletteTransition *transition = &context->transition;
    u16 **bankPointer = &gPaletteBankData;
    s32 destinationOffset = destinationBank * 32;
    u16 *palettes = *bankPointer;
    const u16 *source = &palettes[sourceBank * 16];
    const u16 *destination;
    s32 i;
    transition->destinationBank = destinationBank;
    destination = (void *)((u8 *)palettes + destinationOffset);
    i = 0;
    do {
        s32 color;
        s32 red, green, blue;
        if (transition->remaining != 256)
            color = transition->colors[i];
        else
            color = source[i];
        transition->deltaRed[i] = ((*destination & 31) - (red = color & 31)) << 8;
        transition->deltaGreen[i] = (((*destination >> 5) & 31) - (green = (color >> 5) & 31)) << 8;
        transition->deltaBlue[i] = (((*destination >> 10) & 31) - (blue = (color >> 10) & 31)) << 8;
        transition->deltaRed[i] = Div(transition->deltaRed[i], duration);
        transition->deltaGreen[i] = Div(transition->deltaGreen[i], duration);
        transition->deltaBlue[i] = Div(transition->deltaBlue[i], duration);
        transition->red[i] = red << 8;
        transition->green[i] = green << 8;
        transition->blue[i] = blue << 8;
        destination++;
        i++;
    } while (i < 16);
    transition->remaining = duration;
    {
        u32 mask = 2;
        context->flags = mask | context->flags;
    }
}
