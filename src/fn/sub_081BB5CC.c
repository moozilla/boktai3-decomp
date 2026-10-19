#include "global.h"
#include "solar_bank_panel.h"
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void Bg_SetLayerMap(u32, void *, u16);
s32 MessageWindow_ShowMessage(s32);
void SolarBank_SetState(void *, void (*)(void));
void SolarBank_UpdateSelection(void);
void SpriteMenu_HideAll(void *);
void SolarBank_BeginSelection(struct SolarBankPanel *panel)
{
    Bg_SetLayerMap(0, panel->layerMap, 0);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    SpriteMenu_HideAll(panel);
    panel->label.flags &= ~1;
    panel->controls[0].flags &= ~1;
    panel->controls[1].flags &= ~1;
    panel->selection = 1;
    panel->prompt.flags &= ~1;
    Anim_LoadKeyframe(&panel->prompt, panel->animData, 0x4b, 1);
    MessageWindow_ShowMessage(1);
    SolarBank_SetState(panel, SolarBank_UpdateSelection);
}
