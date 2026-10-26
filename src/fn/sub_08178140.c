#include "global.h"
#include "ram.h"
void SceneHud_RebuildPrimaryOptions(void *);
void PauseMenu_RefreshEquipmentSlotIcons(void *);
void SceneHud_RefreshWidgetVisibility(void *);
void Hud_RefreshEquipmentDisplay(void)
{
    if (gFieldHud && *(u32 *)(gFieldHud + 0xa40)) {
        SceneHud_RebuildPrimaryOptions(gFieldHud);
        PauseMenu_RefreshEquipmentSlotIcons(gFieldHud);
        if (gFieldHud[0xa5e] <= 2)
            SceneHud_RefreshWidgetVisibility(gFieldHud);
    }
}
