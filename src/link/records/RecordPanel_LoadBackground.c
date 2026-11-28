#include "global.h"
#include "engine/video/render.h"
#include "engine/video/palette_resource.h"
#include "ui/widgets/selection_panel.h"
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);

void RecordPanel_LoadBackground(struct SelectionPanel *panel)
{
    u32 value;
    void *resource;
    void *palette;
    resource = ScriptProc_Start(0xC091, 0xCE15);
    panel->backgroundResource = resource;
    value = 1;
    Bg_SetupLayers(2, 0, resource, 0, 0, 1, &value);
    palette = ((struct PaletteResource *)ScriptProc_Start(0x92B3, 0xA2F6))->banks;
    panel->paletteResource = palette;
    CpuSet(palette, gBgPaletteBuffer, 0x100);
}
