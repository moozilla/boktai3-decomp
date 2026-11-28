#include "ui/widgets/selection_panel.h"
s32 GameState_GetChallengeBestRank(u32);

void RecordPanel_SetSelectedDisplayResource(struct SelectionPanel *panel)
{
    panel->entryIds[6] = GameState_GetChallengeBestRank(SelectionPanel_GetSelection(panel)) + 0x22;
    AnimSprite_SelectGlyph(&panel->widgets[6], &panel->widgetResource, panel->entryIds[6]);
}
