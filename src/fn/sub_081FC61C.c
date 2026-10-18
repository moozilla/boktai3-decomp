#include "global.h"

struct Unk081FC61C {
    u16 unk0;
    u16 unk2;
    u8 filler4[2];
    u8 unk6;
    u8 filler7;
    u32 unk8;
    u32 unkC;
    u8 filler10[8];
    u32 unk18;
};

void Memory_Clear(void *, u32);
void Ratatosk_SetAnimatedParticleUpdateCallback(void *, void (*)(void));
void Ratatosk_OnAnimatedParticleUpdate(void);

void Ratatosk_InitAnimatedParticleParams(struct Unk081FC61C *p)
{
    Memory_Clear(p, 0x1C);
    Ratatosk_SetAnimatedParticleUpdateCallback(p, Ratatosk_OnAnimatedParticleUpdate);
    p->unk0 = 0;
    p->unk2 = 20;
    p->unk8 = 5;
    p->unk6 = 0;
    p->unk18 = 0;
    p->unkC = 0x200;
}
