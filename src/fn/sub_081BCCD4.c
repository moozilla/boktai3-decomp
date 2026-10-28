#include "global.h"
#include "input.h"
extern u8 *gGameState, *gUnk_02000580;
void Anim_Step(void *, void *);
void Sound_PlaySong(u32);
void DarkLoan_BeginClosing(void *);
void DarkLoan_FinishOpening(void *);
void DarkLoan_UpdateBalanceDigits(void *);
void StatGauge_DrawPreview(void *);
void DarkLoan_UpdateAdjustmentDigits(void *);
u32 StatGauge_GetSelectionGroup(u32);
void Anim_LoadKeyframe(void *, void *, u32, u32);
void AnimData_SelectPaletteBank(void *, u32);
void MessageWindow_ShowMessage(u32);
#define H(off) (*(u16 *)(s + (off)))
#define W(off) (*(u32 *)(s + (off)))
void DarkLoan_UpdateAdjustment(u8 *s)
{
    u16 *balance, *limit;
    u32 group;
    s32 total;
    u32 *p, *q;
    s32 i;
    Anim_Step(s + 0x58, s + 0x18);
    if (!W(0x8F4)) {
        balance = (u16 *)(gUnk_02000580 + 0x428);
        limit = (u16 *)(gUnk_02000580 + 0x42A);
    } else {
        balance = (u16 *)(gGameState + 0x2C);
        limit = (u16 *)(gGameState + 0x2E);
    }
    if (gPadState[1] & 1) {
        if (!H(0x8EE))
            Sound_PlaySong(0xDE);
        else {
            *balance += H(0x8EE);
            *(u32 *)(gGameState + 0x50C) += H(0x8EE) * H(0x8EC);
            total = *(s32 *)(gGameState + 0x51C) + H(0x8EE) * H(0x8EC);
            if (total < 0)
                total = 0x7FFFFFFF;
            *(s32 *)(gGameState + 0x51C) = total;
            Sound_PlaySong(0xDD);
        }
        DarkLoan_BeginClosing(s);
        return;
    }
    if (gPadState[1] & 2) {
        Sound_PlaySong(0xDE);
        DarkLoan_FinishOpening(s);
        return;
    }
    if ((gPadState[0] & 0x20) && *(s32 *)(gGameState + 0x50C) + (H(0x8EE) + 1) * H(0x8EC) <= 9999 &&
        *balance + H(0x8EE) < *limit) {
        Sound_PlaySong(0x37D);
        H(0x8EE)++;
        DarkLoan_UpdateBalanceDigits(s);
        StatGauge_DrawPreview(s);
        DarkLoan_UpdateAdjustmentDigits(s);
        H(0x900) = 19;
        group = StatGauge_GetSelectionGroup(H(0x8FE));
        Anim_LoadKeyframe(s + 0x238, s + 0x18, (u16)(H(0x900) + group), 1);
        AnimData_SelectPaletteBank(s + 0x8C4, group + 0x1B4);
        if (++H(0x8FE) > 15)
            H(0x8FE) = 0;
    } else if ((gPadState[0] & 0x10) && H(0x8EE)) {
        Sound_PlaySong(0x37D);
        H(0x8EE)--;
        DarkLoan_UpdateBalanceDigits(s);
        StatGauge_DrawPreview(s);
        DarkLoan_UpdateAdjustmentDigits(s);
        H(0x900) = 22;
        group = StatGauge_GetSelectionGroup(H(0x8FE));
        Anim_LoadKeyframe(s + 0x238, s + 0x18, (u16)(H(0x900) + group), 1);
        AnimData_SelectPaletteBank(s + 0x8C4, group + 0x1B4);
        if (++H(0x8FE) > 15)
            H(0x8FE) = 0;
        if (H(0x908))
            H(0x908) = 1;
    } else if (H(0x8FE)) {
        group = StatGauge_GetSelectionGroup(H(0x8FE));
        Anim_LoadKeyframe(s + 0x238, s + 0x18, (u16)(H(0x900) + group), 1);
        AnimData_SelectPaletteBank(s + 0x8C4, group + 0x1B4);
        if (++H(0x8FE) > 15)
            H(0x8FE) = 0;
    } else {
        AnimData_SelectPaletteBank(s + 0x8C4, 0x1B4);
        Anim_LoadKeyframe(s + 0x238, s + 0x18, 18, 1);
    }
    if ((gPadState[0] & 0x20) && *(s32 *)(gGameState + 0x50C) + (H(0x8EE) + 1) * H(0x8EC) > 9999)
        H(0x908) = 60;
    if (H(0x908)) {
        if (--H(0x908) == 0) {
            DarkLoan_UpdateAdjustmentDigits(s);
            MessageWindow_ShowMessage(4);
            H(0x90A) = 0;
        } else {
            u32 mask;
            W(0x5A0) |= 1;
            mask = 1;
            q = (u32 *)(s + 0x600);
            p = (u32 *)(s + 0x420);
            i = 3;
            do {
                *p |= mask;
                *q |= mask;
                q = (void *)((u8 *)q + 0x60);
                p = (void *)((u8 *)p + 0x60);
            } while (--i >= 0);
            if (!H(0x90A)) {
                MessageWindow_ShowMessage(6);
                H(0x90A) = 1;
            }
        }
    }
}
