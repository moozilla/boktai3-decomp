#include "global.h"
struct Pair77 {
    u32 a;
    u32 b;
};
void SpiralBurst_InitParticles(u8 *);
void SpiralBurst_InitCollider(u8 *);
s32 Div(s32, s32);
void Sound_PlaySong(u32);
void SpiralBurst_UpdateParticles(u8 *);
void SpiralBurst_SetState(u8 *, void (*)(u8 *));
s32 SpiralBurst_Init(u8 *p, u8 *owner, struct Pair77 *q)
{
    *(struct Pair77 *)(p + 0x1c) = *q;
    *(u8 **)(p + 0x18) = owner;
    SpiralBurst_InitParticles(p);
    SpiralBurst_InitCollider(p);
    {
        u16 a;
        *(u16 *)(p + 0x1fe) = a = 180;
        *(u16 *)(p + 0x1fc) = Div(a, 3);
    }
    {
        u16 b;
        *(u16 *)(p + 0x200) = b = 900;
        Sound_PlaySong(b + 19);
    }
    SpiralBurst_SetState(p, SpiralBurst_UpdateParticles);
    return 0;
}
