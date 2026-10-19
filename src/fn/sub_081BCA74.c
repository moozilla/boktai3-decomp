#define DarkLoan_BeginAlternateOpening  sub_081BCA74
#define DarkLoan_UpdateAlternateOpening StatGauge_UpdateClosing
#include "global.h"
#include "dark_loan_panel.h"
void StatGauge_ResetBackground(void *);
void DarkLoan_ShowPrompt(void *);
void Bg_SetLayerMap(u32, void *, u16);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void DarkLoan_HideSprites(void *);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
s32 MessageWindow_ShowMessage(s32);
void DarkLoan_SetState(void *, void (*)(void));
void StatGauge_UpdateClosing(void);
void sub_081BCA74(struct DarkLoanPanel *p)
{
    u32 zero;
    StatGauge_ResetBackground(p);
    DarkLoan_ShowPrompt(p);
    Bg_SetLayerMap(0, p->map, 5);
    zero = 0;
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, zero);
    DarkLoan_HideSprites(p);
    p->animationFlags &= ~1;
    Anim_SetKeyframes(p->animation, p->spriteData, 6, 2, zero);
    ScreenTransition_SendCommand(2, 5, 4, 4, 4, 0xe000, zero);
    MessageWindow_ShowMessage(13);
    DarkLoan_SetState(p, StatGauge_UpdateClosing);
}
