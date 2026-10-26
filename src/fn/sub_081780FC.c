#include "global.h"
#include "ram.h"
void sub_08168850(void *);
void SceneHud_UpdateSecondaryWidgets(void *);
void SceneHud_RefreshWidgetVisibility(void *);
void Hud_RefreshInventoryDisplay(void)
{
    if (gFieldHud && *(u32 *)(gFieldHud + 0xa40)) {
        sub_08168850(gFieldHud);
        SceneHud_UpdateSecondaryWidgets(gFieldHud);
        if (gFieldHud[0xa5e] <= 2)
            SceneHud_RefreshWidgetVisibility(gFieldHud);
    }
}
