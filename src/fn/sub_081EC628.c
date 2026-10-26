// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "render.h"
struct ScreenLayerPair {
    u8 unknown0[0x2C0];
    u32 firstFlags;
    u8 unknown2C4[0x44];
    u32 secondFlags;
    u8 unknown30C[0x2E2];
    s16 layer;
};
#define ScreenLayers_ShowPair sub_081EC628
void ScreenLayers_ShowPair(struct ScreenLayerPair *screen)
{
    register u32 mask asm("r0");
    screen->firstFlags &= ~1;
    screen->secondFlags &= ~1;
    if (screen->layer != 0) {
        mask = 1;
    } else {
        mask = 3;
    }
    mask |= gDispCntShadow;
    gDispCntShadow = mask;
}
