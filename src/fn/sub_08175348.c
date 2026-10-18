#include "global.h"
#define MenuSelection_ProcessConfirmation sub_08175348
void Sound_PlaySong(u32);
int sub_08165A30(void *);
void sub_08174F14(u8 *, s32);
void sub_081660D0(u8 *, u32, s32);
void Hud_SetInteractionCallback(u8 *, void (*)(void), u32);
void sub_081663A0(u8 *, u32, u32, u32, u32);
void sub_08175008(void);
void sub_08174F48(void);
void sub_08174E5C(u8 *);
void Player_RefreshStatsAndVitals(void *);
void sub_08174B8C(u8 *);
void sub_08174D20(u8 *);
void Hud_DrawDefaultHealthMeter(u8 *, u32, u32, u32);
void Hud_DrawDefaultEnergyMeter(u8 *, u32, u32, u32, u32, u32);
void sub_08175348(u8 *s)
{
    s32 state = sub_08165A30(s);
    if (state == 0) {
        Sound_PlaySong(0xDE);
        sub_08174F14(s, s[0x48AA] + 20);
        sub_081660D0(s, 1, -1);
        Hud_SetInteractionCallback(s, sub_08175008, 1);
        sub_081663A0(s, 1, 0, 0, 0);
    } else if (state == 1) {
        u8 **p;
        s32 neg;
        u32 count;
        Sound_PlaySong(0xDD);
        sub_08174E5C(s);
        p = (u8 **)(s + 0xA40);
        Player_RefreshStatsAndVitals(*p);
        sub_08174B8C(s);
        sub_08174D20(s);
        Hud_DrawDefaultHealthMeter(s, 1, 8, 8);
        Hud_DrawDefaultEnergyMeter(s, 1, 8, 10, *(u16 *)(*p + 0x428), *(u16 *)(*p + 0x42A));
        neg = -1;
        sub_081660D0(s, 1, neg);
        count = *(u16 *)(s + 0x48A8);
        if (count == 0) {
            *(u32 *)(s + 0x1608) |= state;
            sub_08174F14(s, neg);
            Hud_SetInteractionCallback(s, sub_08174F48, 0);
            sub_081663A0(s, 0, 0, 0, count);
        } else {
            sub_08174F14(s, s[0x48AA] + 20);
            sub_081663A0(s, 1, 0, 0, 0);
            Hud_SetInteractionCallback(s, sub_08175008, 0);
        }
    }
}
