#define SceneHud_SetPairedIcons   sub_081660D0
#define SceneHud_SetWidgetFlags PauseMenu_AddSpriteFlags
#define SceneHud_ClearWidgetFlags PauseMenu_ClearSpriteFlags
#define SceneHud_MapIconKeyframe PauseMenu_GetPromptIconKeyframe
#include "global.h"
#include "scene_hud_display.h"
void PauseMenu_AddSpriteFlags(void *, s32, s32);
void PauseMenu_ClearSpriteFlags(void *, s32, u32);
s32 PauseMenu_GetPromptIconKeyframe(s32);
void sub_081660D0(struct SceneHudState *hud, s32 left, s32 right)
{
    if (left < 0) {
        PauseMenu_AddSpriteFlags(hud, 75, 1);
    } else {
        PauseMenu_ClearSpriteFlags(hud, 75, 1);
        Anim_LoadKeyframe(&hud->valueWidgets[0], hud->widgetResource, (u16)PauseMenu_GetPromptIconKeyframe(left), 0);
    }
    if (right < 0) {
        PauseMenu_AddSpriteFlags(hud, 76, 1);
    } else {
        PauseMenu_ClearSpriteFlags(hud, 76, 1);
        Anim_LoadKeyframe(&hud->valueWidgets[1], hud->widgetResource, (u16)PauseMenu_GetPromptIconKeyframe(right), 0);
    }
}
