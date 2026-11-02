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
void ScreenLayers_HidePair(struct ScreenLayerPair *screen)
{
    screen->firstFlags |= 1;
    screen->secondFlags |= 1;
    if (screen->layer != 0) {
        u16 bits = 1;
        gDispCntShadow &= ~bits;
    } else {
        u16 bits = 3;
        gDispCntShadow &= ~bits;
    }
}
