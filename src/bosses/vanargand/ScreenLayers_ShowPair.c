#include "global.h"
#include "engine/video/render.h"
#include "bosses/vanargand/screen_layer_pair_layout.h"
void ScreenLayers_ShowPair(struct ScreenLayerPair *screen)
{
    screen->firstFlags &= ~1;
    screen->secondFlags &= ~1;
    if (screen->layer != 0) {
        u16 bits = 1;
        gDispCntShadow |= bits;
    } else {
        u16 bits = 3;
        gDispCntShadow |= bits;
    }
}
