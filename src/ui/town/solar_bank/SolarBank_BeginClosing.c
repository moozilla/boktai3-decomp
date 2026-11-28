#include "global.h"
#include "ui/town/solar_bank_panel.h"
void Bg_SetLayerMap(int, void *, int);
void BgGfx_LoadTilemapSimple(int, int, int, int, int);
void SpriteMenu_HideAll(void *);
void Sound_PlaySong(int);
void Anim_SetKeyframes(void *, void *, int, int, int);
void MessageWindow_ShowMessage(int);
void SpriteMenu_UpdateTimedFade(void);
void SolarBank_SetState(void *, void (*)(void));
void SolarBank_BeginClosing(struct SolarBankPanel *panel)
{
    Bg_SetLayerMap(0, panel->layerMap, 2);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    SpriteMenu_HideAll(panel);
    Sound_PlaySong(0x2bf);
    panel->label.flags &= ~1;
    Anim_SetKeyframes(&panel->label, &panel->labelData, 2, 2, 0);
    MessageWindow_ShowMessage(3);
    SolarBank_SetState(panel, SpriteMenu_UpdateTimedFade);
}
