#include "global.h"
void Memory_Clear(void *, u32);
void SpriteParticle_SetMotionParams(void *, u32, u32, u32, u32);
void SpriteParticle_SetSpriteResource(void *, u32, u32, s32);
void SpriteParticle_SetUpdateCallback(void *, void (*)(void));
void SpriteParticle_OnDefaultUpdate(void);
struct S {
    u8 p[0x2c];
    u32 w2c;
    u16 h30;
};
void SpriteParticle_InitParams(struct S *p)
{
    u32 z;
    Memory_Clear(p, 0x34);
    SpriteParticle_SetMotionParams(p, 0, 0x78, 0, 5);
    SpriteParticle_SetSpriteResource(p, 0x848F, 0, -1);
    SpriteParticle_SetUpdateCallback(p, SpriteParticle_OnDefaultUpdate);
    z = 0;
    p->w2c = z;
    p->h30 = z;
}
