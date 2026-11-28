#include "global.h"
#include "engine/video/render.h"
#include "ui/town/solar_bank_panel.h"
void *ScriptProc_Start(u16, u16);
void SpriteMenu_LoadPalette(struct SolarBankPanel *panel)
{
    panel->paletteResource = (u8 *)ScriptProc_Start(0x92B3, 0x2E9) + 0x1B4;
    CpuSet(panel->paletteResource, gBgPaletteBank13, 0x04000018);
    CpuSet(gPaletteBankData + 0x7540, panel->promptPalette, 0x04000008);
    panel->paletteState = 0;
}
