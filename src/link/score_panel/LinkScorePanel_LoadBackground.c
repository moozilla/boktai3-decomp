#include "global.h"
#include "engine/video/palette_resource.h"
#include "engine/video/render.h"
#include "ui/save/save_menu.h"
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);

void LinkScorePanel_LoadBackground(struct SaveMenu *menu)
{
    u32 value;
    void *palette;
    menu->backgroundResource = ScriptProc_Start(0xC091, 0xCE15);
    menu->alternateBackgroundResource = ScriptProc_Start(0xC091, 0xA597);
    value = 3;
    Bg_SetupLayers(2, 0, menu->backgroundResource, 0, 0, 1, &value);
    palette = ((struct PaletteResource *)ScriptProc_Start(0x92B3, 0xA2F6))->banks;
    menu->paletteResource = palette;
    CpuSet(palette, gBgPaletteBuffer, 0x100);
}
