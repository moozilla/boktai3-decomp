
#ifdef NONMATCHING
// fuzzy: 86.8%
// foundry draft (worker best 86.8%); what differs is mostly register allocation and instruction order
#define ScreenLayers_HidePair sub_081EC670
#define gBackgroundVisibility gUnk_03004BD8
#include "global.h"
struct ScreenLayerPair {
    u8 unknown0[0x2C0];
    u32 firstFlags;
    u8 unknown2C4[0x44];
    u32 secondFlags;
    u8 unknown30C[0x2E2];
    s16 layer;
};
extern u16 gUnk_03004BD8;
void sub_081EC670(struct ScreenLayerPair *screen)
{
    s32 mask;
    u16 *control;
    screen->firstFlags |= 1;
    screen->secondFlags |= 1;
    if (screen->layer != 0) {
        control = &gUnk_03004BD8;
        mask = ~1;
    } else {
        control = &gUnk_03004BD8;
        mask = ~3;
    }
    {
        u16 value = *control;
        mask &= value;
        *control = mask;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081EC670);
#endif
