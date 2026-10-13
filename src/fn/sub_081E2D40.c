#include "global.h"
// Provisional name: Presentation_Destroy
extern u16 gUnk_03004BD8;
void sub_082195E0(void *);
// Release all five presentation layers and restore display-control bits.
s32 Presentation_Destroy(u8 *p)
{
    u8 *layer;
    s32 i;
    {
        s32 value = gUnk_03004BD8;
        s32 mask = -3;
        mask &= value;
        {
            s32 mask2 = -4;
            mask &= mask2;
            gUnk_03004BD8 = mask;
        }
    }
    layer = p + 0x38;
    for (i = 4; i >= 0; i--) {
        sub_082195E0(layer);
        layer += 0x60;
    }
    return 0;
}
