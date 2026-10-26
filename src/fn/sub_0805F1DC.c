#include "scene_input_panel.h"
void Palette_CopyBank(u16 *, const u16 *);
void SceneInputPanel_InitHighlight(struct SceneInputPanel *panel,
                                   struct SceneInputHighlight *highlight)
{
    Palette_CopyBank(highlight->colors, (u16 *)(gPaletteBankData + 0x7540));
    highlight->phase = 0;
    SceneInputPanel_ResetHighlights(panel, highlight);
}
