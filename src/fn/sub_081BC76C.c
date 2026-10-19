#define DarkLoan_FinishOpening sub_081BC76C
#include "global.h"
#include "dark_loan_panel.h"
void Bg_SetLayerMap(u32, void *, u16);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void DarkLoan_HideSprites(void *);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
s32 MessageWindow_ShowMessage(s32);
void DarkLoan_SetState(void *, void (*)(void));
void StatGauge_UpdateConfirmation(void);
void sub_081BC76C(struct DarkLoanPanel *p)
{
    u32 zero;
    Bg_SetLayerMap(0, p->map, 3);
    zero = 0;
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, zero);
    DarkLoan_HideSprites(p);
    p->animationFlags &= ~1;
    Anim_SetKeyframes(p->animation, p->spriteData, 7, 2, zero);
    p->spriteFlags120 &= ~1;
    p->spriteFlags180 &= ~1;
    p->spriteFlags1e0 &= ~1;
    p->secondaryVisible = 1;
    p->secondaryAnimationFlags &= ~1;
    Anim_LoadKeyframe(p->secondaryAnimation, p->secondarySpriteData, 0x4b, 1);
    MessageWindow_ShowMessage(1);
    p->selection = zero;
    DarkLoan_SetState(p, StatGauge_UpdateConfirmation);
}
