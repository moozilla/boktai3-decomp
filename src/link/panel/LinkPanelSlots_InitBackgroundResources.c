#include "global.h"
#include "engine/video/render.h"
#include "engine/video/palette_resource.h"
#include "gba/syscall.h"
#include "link/panel/link_panel.h"
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);
void LinkPanelSlots_InitBackgroundResources(struct LinkPanel *panel)
{
    u32 maps[1];
    void *resource;
    u16 *palette;

    resource = ScriptProc_Start(0xC091, 0xCE15);
    panel->backgroundResource = resource;
    maps[0] = 2;
    Bg_SetupLayers(2, 0, resource, 0, 0, 1, maps);
    palette = ((struct PaletteResource *)ScriptProc_Start(0x92B3, 0xA2F6))->banks;
    panel->paletteResource = palette;
    CpuCopy16(palette, gBgPaletteBuffer, 0x200);
}
