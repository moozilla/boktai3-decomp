#include "scene_input_panel.h"
#define SceneInputPanel_HighlightPrompt sub_0805F2CC
extern u8 *gUnk_030042E4;
void SceneInputPanel_HighlightPrompt(struct SceneInputPanel *panel, u16 *palette)
{
    u16 *normal = (u16 *)(gUnk_030042E4 + 0x7540);
    panel->compareA.palette = normal;
    panel->compareA.paletteBank = 0x3AA;
    panel->modeCursor.palette = normal;
    panel->modeCursor.paletteBank = 0x3AA;
    panel->prompt.palette = palette;
    panel->prompt.paletteBank = 0x8000;
}
