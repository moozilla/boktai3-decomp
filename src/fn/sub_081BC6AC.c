#include "global.h"
#include "dark_loan_panel.h"
void StatGauge_ResetBackground(void *);
void DarkLoan_ShowPrompt(void *);
void Bg_SetLayerMap(u32, void *, u16);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void DarkLoan_HideSprites(void *);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
s32 MessageWindow_ShowMessage(s32);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void DarkLoan_FinishOpening(void *);
void DarkLoan_UpdateOpeningAnimation(void);
void DarkLoan_SetState(void *, void (*)(void));
void DarkLoan_BeginOpening(struct DarkLoanPanel *p)
{
    u32 zero;
    StatGauge_ResetBackground(p);
    DarkLoan_ShowPrompt(p);
    Bg_SetLayerMap(0, p->map, 3);
    zero = 0;
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, zero);
    DarkLoan_HideSprites(p);
    p->animationFlags &= ~1;
    Anim_SetKeyframes(p->animation, p->spriteData, 4, 2, zero);
    p->spriteFlags120 &= ~1;
    p->spriteFlags180 &= ~1;
    p->spriteFlags1e0 &= ~1;
    MessageWindow_ShowMessage(0);
    ScreenTransition_SendCommand(2, 5, 4, 4, 4, 0xe000, zero);
    if (!p->animateOpening)
        DarkLoan_FinishOpening(p);
    else
        DarkLoan_SetState(p, DarkLoan_UpdateOpeningAnimation);
}
