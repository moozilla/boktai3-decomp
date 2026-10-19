#include "global.h"
#include "solar_bank_panel.h"
#define SolarBank_BeginOpening sub_081BB538
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void Bg_SetLayerMap(u32, void *, u16);
s32 MessageWindow_ShowMessage(s32);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void SolarBank_SetState(void *, void (*)(void));
void SolarBank_UpdateOpening(void);
void SpriteMenu_HideAll(void *);
void sub_081BB538(struct SolarBankPanel *panel)
{
    Bg_SetLayerMap(0, panel->layerMap, 0);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    SpriteMenu_HideAll(panel);
    panel->label.flags &= ~1;
    Anim_SetKeyframes(&panel->label, panel->labelData, 0, 2, 0);
    panel->controls[0].flags &= ~1;
    panel->controls[1].flags &= ~1;
    MessageWindow_ShowMessage(0);
    ScreenTransition_SendCommand(0, 5, 4, 4, 4, 0xffff, 0);
    SolarBank_SetState(panel, SolarBank_UpdateOpening);
}
