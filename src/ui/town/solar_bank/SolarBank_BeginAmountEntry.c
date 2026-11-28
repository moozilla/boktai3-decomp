#include "global.h"
#include "ui/town/solar_bank_panel.h"
#include "system/game_state/game_state.h"
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
void AnimData_SelectPaletteBank(void *, u32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void Bg_SetLayerMap(u32, void *, u16);
s32 MessageWindow_ShowMessage(s32);
void SolarBank_SetState(void *, void (*)(void));
void SolarBank_RefreshAmountDigits(struct SolarBankPanel *);
void SpriteMenu_HideAll(void *);
void SolarBank_UpdateTransferInput(void);
void SolarBank_BeginAmountEntry(struct SolarBankPanel *panel)
{
    s32 i;
    Bg_SetLayerMap(0, panel->layerMap, 1);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    SpriteMenu_HideAll(panel);
    panel->label.flags &= ~1;
    Anim_SetKeyframes(&panel->label, &panel->labelData, 1, 1, 0);
    panel->percentSign.flags &= ~1;
    Anim_LoadKeyframe(&panel->percentSign, &panel->labelData, 0x12, 1);
    panel->balance = gGameState->unk77C;
    panel->transferAmount = gGameState->unk7B0;
    for (i = 0; i < 4; i++) {
        panel->balanceDigits[i].flags &= ~1;
        panel->transferDigits[i].flags &= ~1;
    }
    SolarBank_RefreshAmountDigits(panel);
    AnimSprite_Show(&panel->transferLabel);
    for (i = 0; i < 2; i++)
        panel->sprites[i].flags &= ~1;
    AnimData_SelectPaletteBank(&panel->indicators[0], 0x1AE);
    AnimData_SelectPaletteBank(&panel->indicators[1], 0x1B1);
    AnimSprite_Show(&panel->firstMarker);
    AnimSprite_Show(&panel->secondMarker);
    MessageWindow_ShowMessage(2);
    panel->holdFrames = 0;
    panel->repeatSpeed = 0;
    panel->animationTimer = 0;
    panel->transferDirection = 0;
    panel->blinkPhase = 0;
    SolarBank_SetState(panel, SolarBank_UpdateTransferInput);
}
