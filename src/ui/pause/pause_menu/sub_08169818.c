#include "global.h"

void sub_08168850(void *);
void SceneHud_UpdateSecondaryWidgets(void *);
void SceneHud_RefreshWidgetVisibility(void *);
void SceneHud_ApplySecondarySelection(void *);

void PauseMenu_RefreshEquipmentPanel(void *p)
{
    sub_08168850(p);
    SceneHud_UpdateSecondaryWidgets(p);
    SceneHud_RefreshWidgetVisibility(p);
    SceneHud_ApplySecondarySelection(p);
}
