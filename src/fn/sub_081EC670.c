// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "render.h"

#define gBackgroundVisibility gDispCntShadow
struct ScreenLayerPair {
    u8 unknown0[0x2C0];
    u32 firstFlags;
    u8 unknown2C4[0x44];
    u32 secondFlags;
    u8 unknown30C[0x2E2];
    s16 layer;
};
void ScreenLayers_HidePair(struct ScreenLayerPair *screen)
{
    register s32 mask asm("r1");
    register u16 *control asm("r0");
    screen->firstFlags |= 1;
    screen->secondFlags |= 1;
    if (screen->layer != 0) {
        control = &gDispCntShadow;
        mask = ~1;
    } else {
        control = &gDispCntShadow;
        mask = ~3;
    }
    {
        u16 value = *control;
        mask &= value;
        *control = mask;
    }
}
