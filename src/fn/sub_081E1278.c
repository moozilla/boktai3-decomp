#include "global.h"
#include "render.h"
// Names are tentative, based on the static state transitions and memory writes.
struct SelectionPalette {
    u8 unknown[0x4cc];
    s32 selection;
    u8 unknown4D0[0x4e8 - 0x4d0];
    s32 pulseTimer;
};
void SelectionPalette_UpdatePulse(struct SelectionPalette *p)
{
    u16 first = 0, second = 0;
    u16 *color;
    s32 base = 10;
    switch (p->selection) {
        case 0:
            color = &first;
            break;
        case 1:
            color = &second;
            break;
        default:
            return;
    }
    if (p->pulseTimer < base)
        *color = 31;
    else if (p->pulseTimer < base + 10)
        *color = 31;
    else if (p->pulseTimer < base + 18)
        *color = 27;
    else if (p->pulseTimer < base + 26)
        *color = 18;
    else if (p->pulseTimer < base + 34)
        *color = 10;
    else if (p->pulseTimer < base + 42)
        *color = 18;
    else
        *color = 27;
    if (++p->pulseTimer >= base + 50)
        p->pulseTimer = base;
    gBgPaletteBuffer[45] = first;
    gBgPaletteBuffer[44] = second;
}
