#include "global.h"
#ifdef NONMATCHING
// fuzzy: 67.5%
// 9 differing instruction runs; first diff at +0x14 (ldr r2, [pc, #0x7c] vs ldr r1, [pc, #0x78])
extern u8 *gUnk_02000710;
u32 LinkPlayer_GetSessionPlayerData(u8 *);
void LinkPlayer_InitOptionalPlayerState(u8 *);
void LinkPlayer_LoadBaseStats(u8 *);
void LinkPlayer_InitHealth(u8 *);
void LinkPlayer_InitEnergy(u8 *);
void Player_SetTetherMode(u8 *, s32, s32);
void Player_InitializeInputHistory(u8 *);
s32 sub_0823ADB4(u8 *p)
{
    u32 v = LinkPlayer_GetSessionPlayerData(p);
    *(u32 *)(p + 0x344) = v;
    *(u32 *)(p + 0x1c) = 1;
    *(u32 *)(gUnk_02000710 + 0x628) = 0;
    *(u32 *)(gUnk_02000710 + 0x62c) = 0;
    *(u16 *)(gUnk_02000710 + 0x610) = 0;
    *(u16 *)(gUnk_02000710 + 0x612) = 0;
    LinkPlayer_InitOptionalPlayerState(p);
    LinkPlayer_LoadBaseStats(p);
    LinkPlayer_InitHealth(p);
    LinkPlayer_InitEnergy(p);
    Player_SetTetherMode(p, 0, 0);
    Player_InitializeInputHistory(p);
    *(u16 *)(p + 0x534) = 0;
    *(u16 *)(p + 0x536) = 0;
    *(u16 *)(p + 0x538) = 0;
    *(u8 *)(p + 0x549) = *(u32 *)(p + 0x18);
    *(u16 *)(p + 0x54e) = 0;
    *(u32 *)(p + 0x550) = 0;
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0823ADB4);
#endif
