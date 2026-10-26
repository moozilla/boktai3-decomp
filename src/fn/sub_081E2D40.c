#include "global.h"
#include "render.h"
// Provisional name: Presentation_Destroy
void Anim_Remove(void *);
// Release all five presentation layers and restore display-control bits.
s32 Presentation_Destroy(u8 *p)
{
    u8 *layer;
    s32 i;
    {
        s32 value = gDispCntShadow;
        s32 mask = -3;
        mask &= value;
        {
            s32 mask2 = -4;
            mask &= mask2;
            gDispCntShadow = mask;
        }
    }
    layer = p + 0x38;
    for (i = 4; i >= 0; i--) {
        Anim_Remove(layer);
        layer += 0x60;
    }
    return 0;
}
