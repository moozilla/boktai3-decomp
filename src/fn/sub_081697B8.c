#define SelectionPanel_RefreshPrimaryEntries PauseMenu_RefreshEquipmentSlotIcons
#include "global.h"
#include "engine.h"
struct SelectionPanel {
    u8 pad[0xA40];
    void *owner;
};
void PauseMenu_CreateSelectionWidgets(struct SelectionPanel *);
void sub_08168850(struct SelectionPanel *);
void SceneHud_RebuildPrimaryOptions(struct SelectionPanel *);
void SceneHud_RebuildTertiaryOptions(struct SelectionPanel *);
void SceneHud_UpdateSecondaryWidgets(struct SelectionPanel *);
void PauseMenu_RefreshEquipmentSlotIcons(struct SelectionPanel *);
void SceneHud_UpdateTertiaryWidgets(struct SelectionPanel *);
void Hud_SetElementGroupFlags(struct SelectionPanel *);
void SceneHud_RefreshWidgetVisibility(struct SelectionPanel *);
void SelectionPanel_RefreshAttachedPanel(struct SelectionPanel *p)
{
    PauseMenu_CreateSelectionWidgets(p);
    if (p->owner) {
        sub_08168850(p);
        SceneHud_RebuildPrimaryOptions(p);
        SceneHud_RebuildTertiaryOptions(p);
        SceneHud_UpdateSecondaryWidgets(p);
        PauseMenu_RefreshEquipmentSlotIcons(p);
        SceneHud_UpdateTertiaryWidgets(p);
        if (gEngineFlags & 0x1000)
            Hud_SetElementGroupFlags(p);
        else
            SceneHud_RefreshWidgetVisibility(p);
    }
}
