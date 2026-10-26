#include "scene_input_panel.h"
#include "render.h"
void SceneInputPanel_HighlightMode(struct SceneInputPanel *panel, u16 *palette)
{
    u16 *normal = (u16 *)(gPaletteBankData + 0x7540);
    panel->compareA.palette = normal;
    panel->compareA.paletteBank = 0x3AA;
    panel->modeCursor.palette = palette;
    panel->modeCursor.paletteBank = 0x8000;
    panel->prompt.palette = normal;
    panel->prompt.paletteBank = 0x3AA;
}
