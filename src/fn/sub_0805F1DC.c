#include "scene_input_panel.h"
#define SceneInputPanel_InitHighlight sub_0805F1DC
void sub_08217DB4(u16 *, const u16 *);
void SceneInputPanel_InitHighlight(struct SceneInputPanel *panel,
                                   struct SceneInputHighlight *highlight)
{
    sub_08217DB4(highlight->colors, (u16 *)(gUnk_030042E4 + 0x7540));
    highlight->phase = 0;
    sub_0805F324(panel, highlight);
}
