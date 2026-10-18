#include "global.h"
extern u8 *gUnk_02000214;
void sub_08168850(void *);
void SceneHud_UpdateSecondaryWidgets(void *);
void SceneHud_RefreshWidgetVisibility(void *);
void Hud_RefreshInventoryDisplay(void)
{
    if (gUnk_02000214 && *(u32 *)(gUnk_02000214 + 0xa40)) {
        sub_08168850(gUnk_02000214);
        SceneHud_UpdateSecondaryWidgets(gUnk_02000214);
        if (gUnk_02000214[0xa5e] <= 2)
            SceneHud_RefreshWidgetVisibility(gUnk_02000214);
    }
}
