#include "ui/pause/scene_hud_display.h"
#define SelectionPanel_RefreshPrimaryEntries PauseMenu_RefreshEquipmentSlotIcons
#include "global.h"
#include "engine/engine.h"
void PauseMenu_CreateSelectionWidgets(struct SceneHudState *);
void sub_08168850(struct SceneHudState *);
void SceneHud_RebuildPrimaryOptions(struct SceneHudState *);
void SceneHud_RebuildTertiaryOptions(struct SceneHudState *);
void SceneHud_UpdateSecondaryWidgets(struct SceneHudState *);
void PauseMenu_RefreshEquipmentSlotIcons(struct SceneHudState *);
void SceneHud_UpdateTertiaryWidgets(struct SceneHudState *);
void Hud_SetElementGroupFlags(struct SceneHudState *);
void SceneHud_RefreshWidgetVisibility(struct SceneHudState *);
void SelectionPanel_RefreshAttachedPanel(struct SceneHudState *p)
{
    PauseMenu_CreateSelectionWidgets(p);
    if (p->actionOwner) {
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
