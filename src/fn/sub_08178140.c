#include "global.h"
extern u8 *gUnk_02000214;
void sub_08168A0C(void *);
void PauseMenu_RefreshEquipmentSlotIcons(void *);
void SceneHud_RefreshWidgetVisibility(void *);
void Hud_RefreshEquipmentDisplay(void)
{
    if (gUnk_02000214 && *(u32 *)(gUnk_02000214 + 0xa40)) {
        sub_08168A0C(gUnk_02000214);
        PauseMenu_RefreshEquipmentSlotIcons(gUnk_02000214);
        if (gUnk_02000214[0xa5e] <= 2)
            SceneHud_RefreshWidgetVisibility(gUnk_02000214);
    }
}
