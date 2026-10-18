#include "global.h"
struct O08239CDC {
    u8 f0[6];
    u16 f6;
    u8 f8[4];
    u32 fC;
};
struct P08239CDC {
    u8 f0[0xC6];
    u16 fC6;
    u8 fC8[0xC];
    u32 fD4;
    u8 fD8[0x178 - 0xD8];
    struct O08239CDC *f178;
    u8 f17C[0xAB0 - 0x17C];
    u16 fAB0;
    u8 fAB2[0xAC4 - 0xAB2];
    u16 fAC4;
    u8 fAC6[2];
    u8 fAC8;
};
void LinkPlayer_InitAnimationSlots(void *);
void sub_08239A20(void *, s32);
void LinkPlayer_InitAnimationState(struct P08239CDC *p)
{
    LinkPlayer_InitAnimationSlots(p);
    p->fAC4 = 0xFFFF;
    p->fAC8 = 0xFF;
    p->fC6 = p->fAB0;
    p->fD4 = (u32)p + 0x360;
    p->f178->f6 = p->fAB0;
    p->f178->fC = (u32)p + 0x360;
    sub_08239A20(p, 0);
}
