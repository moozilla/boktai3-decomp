#include "scene_input_panel.h"
#define SceneInputPanel_ResetHighlights sub_0805F324
extern u8 *gUnk_030042E4;
void SceneInputPanel_ResetHighlights(struct SceneInputPanel *panel,
                                     struct SceneInputHighlight *unused)
{
    u16 *normal = (u16 *)(gUnk_030042E4 + 0x7540);
    panel->compareA.palette = normal;
    panel->compareA.paletteBank = 0x3AA;
    panel->modeCursor.palette = normal;
    panel->modeCursor.paletteBank = 0x3AA;
    panel->prompt.palette = normal;
    panel->prompt.paletteBank = 0x3AA;
}
