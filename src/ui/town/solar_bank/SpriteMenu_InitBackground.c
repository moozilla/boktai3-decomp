#include "global.h"
#include "ui/town/solar_bank_panel.h"
void *ScriptProc_Start(u16, u16);
void Bg_SetupLayers(int, int, void *, int, int, int, void *);
void SpriteMenu_InitBackground(struct SolarBankPanel *panel)
{
    s32 zero;
    panel->layerMap = ScriptProc_Start(0xC091, 0x8544);
    zero = 0;
    Bg_SetupLayers(0, 0, panel->layerMap, 0, 0, 1, &zero);
}
