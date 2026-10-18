#include "global.h"
#ifdef NONMATCHING
// fuzzy: 67.1%
// 9 differing instruction runs; size -4 bytes; first diff at +0x46 (ldr r0, [pc, #0x48] vs ldr r0, [pc, #0x44])
void sub_0823A060(u8 *);
void LinkPlayer_AdvanceActionTick(u8 *);
s32 sub_08239F74(u8 *, s32);
s32 sub_08239EB4(u8 *, s32);
s32 sub_0823A028(u8 *, s32);
void sub_08239A20(u8 *, s32);
void sub_082386DC(u8 *);
void Player_UpdateOrbitingAuraEffect(u8 *);
void sub_08238FE0(u8 *);
void Player_UpdateConvergingParticles(u8 *);
void Callback_InvokeActor(u8 *, u32);
void sub_0823A2E4(u8 *p)
{
    sub_0823A060(p);
    LinkPlayer_AdvanceActionTick(p);
    sub_08239A20(p, sub_0823A028(p, sub_08239EB4(p, sub_08239F74(p, 0))));
    sub_082386DC(p);
    Player_UpdateOrbitingAuraEffect(p);
    sub_08238FE0(p);
    Player_UpdateConvergingParticles(p);
    {
        u32 *source = (u32 *)(p + 0x864);
        u8 *target = p + 0x7fc;
        Callback_InvokeActor(target, *source);
    }
    {
        u32 v = *(u16 *)(p + 0x6f8);
        u32 add = *(u16 *)(p + 0x30);
        *(u16 *)(p + 0x6f0) = v + add;
    }
    {
        u32 v = *(u16 *)(p + 0x6fa);
        u32 add = *(u16 *)(p + 0x32);
        *(u16 *)(p + 0x6f2) = v + add;
    }
    {
        u32 v = *(u16 *)(p + 0x6fc);
        u32 add = *(u16 *)(p + 0x34);
        *(u16 *)(p + 0x6f4) = v + add;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0823A2E4);
#endif
