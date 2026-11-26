#include "global.h"
void Memory_Clear(void *, u32);
void VanargandParticle_SetMotionParams(void *, u32, u32, u32, u32);
void VanargandParticle_SetSpriteResource(void *, u32, u32, s32);
void VanargandParticle_SetUpdateCallback(void *, void (*)(void));
void VanargandParticle_OnDefaultUpdate(void);
struct S {
    u8 p[0x2c];
    u32 w2c;
    u16 h30;
};
void VanargandParticle_InitParams(struct S *p)
{
    Memory_Clear(p, 0x34);
    VanargandParticle_SetMotionParams(p, 0, 0x78, 0, 5);
    VanargandParticle_SetSpriteResource(p, 0x848F, 0, -1);
    VanargandParticle_SetUpdateCallback(p, VanargandParticle_OnDefaultUpdate);
    p->w2c = 0;
    p->h30 = 0;
}
