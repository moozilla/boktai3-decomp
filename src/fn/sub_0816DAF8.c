#define SceneHud_ReturnToActionGrid sub_0816DAF8
#define SceneHud_RefreshActionIcons sub_0816D98C
#define SceneHud_ClearWidgetFlags PauseMenu_ClearSpriteFlags
#define SceneHud_SetWidgetFlags PauseMenu_AddSpriteFlags
#define SceneHud_GetActionIconPosition sub_0816D920
#include "global.h"
#include "scene_hud_display.h"
void SceneHud_RefreshActionIcons(void *);
void Hud_RestoreSecondaryGridSelection(void *);
void sub_0816CDB8(void *, s32, s32);
void SceneHud_ClearWidgetFlags(void *, s32, u32);
void SceneHud_SetWidgetFlags(void *, s32, u32);
void SceneHud_GetActionIconPosition(void *, s32);
void sub_0816D868(void *);
void sub_081672FC(void *);
void sub_08166B7C(void *, s32);
void sub_08166534(void *, s32, s32);
void sub_081663A0(void *, s32, s32, s32, s32);
void Hud_SetInteractionCallback(void *, void (*)(void), s32);
void sub_0816FCD0(void);
void sub_0816DAF8(struct SceneHudState *s)
{
    void *input;
    SceneHud_RefreshActionIcons(s);
    input = s->actionInput;
    Hud_RestoreSecondaryGridSelection(input);
    if (s->mode == 1)
        sub_0816CDB8(input, 1, 0xe1);
    else
        sub_0816CDB8(input, 1, 0x61);
    SceneHud_ClearWidgetFlags(s, 4, 1);
    SceneHud_SetWidgetFlags(s, 6, 1);
    SceneHud_SetWidgetFlags(s, 5, 1);
    SceneHud_GetActionIconPosition(&s->categoryWidgets[4].position, s->actionCursor);
    sub_0816D868(s);
    sub_081672FC(s);
    sub_08166B7C(s, 4);
    sub_08166534(s, 10, 6);
    sub_081663A0(s, 0, 1, 0, 0);
    Hud_SetInteractionCallback(s, sub_0816FCD0, 0);
}
