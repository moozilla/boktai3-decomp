#include "global.h"
#include "solar_bank_panel.h"
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
void AnimData_SelectPaletteBank(void *, u32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void Bg_SetLayerMap(u32, void *, u16);
s32 MessageWindow_ShowMessage(s32);
void SolarBank_SetState(void *, void (*)(void));
void SolarBank_RefreshAmountDigits(struct SolarBankPanel *);
void SpriteMenu_HideAll(void *);
void sub_081BB904(void);
extern u8 *gGameState;
#define FIELD(T, off) (*(T *)((u8 *)panel + (off)))
void SolarBank_BeginAmountEntry(struct SolarBankPanel *panel)
{
    u32 *a, *b;
    s32 i;
    u16 zero;
    Bg_SetLayerMap(0, panel->layerMap, 1);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    SpriteMenu_HideAll(panel);
    panel->label.flags &= ~1;
    Anim_SetKeyframes(&panel->label, panel->labelData, 1, 1, 0);
    FIELD(u32, 0x1E0) &= ~1;
    Anim_LoadKeyframe(panel->unk1D8, panel->labelData, 0x12, 1);
    panel->balance = *(u32 *)(gGameState + 0x77C);
    panel->transferAmount = *(s16 *)(gGameState + 0x7B0);
    b = &panel->transferDigits[0].flags;
    a = &panel->balanceDigits[0].flags;
    for (i = 3; i >= 0; i--) {
        *a &= ~1;
        *b &= ~1;
        b += 24;
        a += 24;
    }
    SolarBank_RefreshAmountDigits(panel);
    FIELD(struct SolarBankDigit, 0x538).flags &= ~1;
    a = &FIELD(u32, 0x658);
    for (i = 1; i >= 0; i--) {
        *a &= ~1;
        a += 11;
    }
    zero = 0;
    AnimData_SelectPaletteBank((u8 *)panel + 0x6B0, 0x1AE);
    AnimData_SelectPaletteBank((u8 *)panel + 0x6CC, 0x1B1);
    FIELD(struct SolarBankDigit, 0x598).flags &= ~1;
    FIELD(struct SolarBankDigit, 0x5F8).flags &= ~1;
    MessageWindow_ShowMessage(2);
    FIELD(u16, 0x702) = zero;
    FIELD(u16, 0x704) = zero;
    FIELD(u16, 0x706) = zero;
    FIELD(u16, 0x708) = zero;
    FIELD(u16, 0x700) = zero;
    SolarBank_SetState(panel, sub_081BB904);
}
