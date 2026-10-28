#include "global.h"
#include "render.h"
extern u32 gUnk_030042C0[];
extern u32 gUnk_030042C8[];
extern u32 gUnk_030042D0[];
void ObjGfx_ResetAllocationCounters(void);
void SpriteOam_ResetQueue(u32);
void Sprite_InitSystem(void)
{
    s32 i;
    u32 z;
    u32 *a, *b, *c;
    gCamera.position.x = 0;
    gCamera.position.y = 0;
    gCamera.position.z = 0;
    gCamera.unk0C = 1;
    gCamera.unk10 = 0;
    gRenderListIndex = 0;
    z = 0;
    c = gUnk_030042C8;
    b = gUnk_030042D0;
    a = gUnk_030042C0;
    for (i = 1; i >= 0; i--) {
        *a++ = z;
        *b++ = z;
        *c++ = z;
    }
    ObjGfx_ResetAllocationCounters();
    SpriteOam_ResetQueue(1);
}
