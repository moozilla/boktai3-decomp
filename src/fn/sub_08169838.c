
#include "global.h"
struct SelectionPanel {
    u8 pad[0xF4A];
    u8 selection;
};
void SceneHud_RebuildTertiaryOptions(struct SelectionPanel *);
void SceneHud_UpdateTertiaryWidgets(struct SelectionPanel *);
void SceneHud_RefreshWidgetVisibility(struct SelectionPanel *);
void PauseMenu_ApplySelectedVariant(struct SelectionPanel *);
void SelectionPanel_ResetSecondarySelection(struct SelectionPanel *p)
{
    p->selection = 0;
    SceneHud_RebuildTertiaryOptions(p);
    SceneHud_UpdateTertiaryWidgets(p);
    SceneHud_RefreshWidgetVisibility(p);
    PauseMenu_ApplySelectedVariant(p);
}
