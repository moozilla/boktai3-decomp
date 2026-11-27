#include "global.h"
struct Pair33A {
    u32 a, b;
};
void ChargedBurst_InitCoreEffect(u8 *);
void ChargedBurst_InitParticles(u8 *);
void ChargedBurst_InitCollider(u8 *, u32, u32, u32);
void Sound_PlaySong(u32);
void TimedDisplay_AdvanceIntro(u8 *);
void ChargedBurst_SetState(u8 *, void (*)(u8 *));
s32 ChargedBurst_Init(u8 *p, struct Pair33A *q, u32 a, u32 b, u32 c, u32 d)
{
    *(struct Pair33A *)(p + 0x18) = *q;
    ChargedBurst_InitCoreEffect(p);
    ChargedBurst_InitParticles(p);
    ChargedBurst_InitCollider(p, a, b, c);
    *(u16 *)(p + 0x206) = 0;
    *(u16 *)(p + 0x208) = d;
    Sound_PlaySong(920);
    ChargedBurst_SetState(p, TimedDisplay_AdvanceIntro);
    return 0;
}
