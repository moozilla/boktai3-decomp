#include "global.h"
#include "scene_hud_display.h"
#define SceneHud_RefreshControls sub_081782E0
#define gSceneHud                gUnk_02000214
extern struct SceneHudState *gSceneHud;
extern struct Player *gUnk_02000580[];
extern u32 gUnk_030053F4;
void sub_08168A0C(struct SceneHudState *);
void PauseMenu_RefreshEquipmentSlotIcons(struct SceneHudState *);
void sub_08168850(struct SceneHudState *);
void sub_081686FC(struct SceneHudState *);
void sub_08169BE0(struct SceneHudState *);
void sub_08168534(struct SceneHudState *);
void sub_081685B4(struct SceneHudState *);
void sub_08167BDC(struct SceneHudState *);
void sub_081782E0(void)
{
    if (gSceneHud) {
        struct Player *owner = gUnk_02000580[0];
        gSceneHud->actionOwner = owner;
        if (gSceneHud->actionOwner) {
            sub_08168A0C(gSceneHud);
            PauseMenu_RefreshEquipmentSlotIcons(gSceneHud);
            sub_08168850(gSceneHud);
            sub_081686FC(gSceneHud);
            if (gUnk_030053F4 & 0x1000) {
                if (!gSceneHud->controlMode)
                    sub_08169BE0(gSceneHud);
                sub_08168534(gSceneHud);
            } else if (!gSceneHud->controlMode) {
                sub_081685B4(gSceneHud);
                sub_08167BDC(gSceneHud);
            }
        }
    }
}
