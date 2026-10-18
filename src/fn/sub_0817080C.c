#include "global.h"
#include "scene_hud_display.h"
void PauseMenu_AddSpriteFlags(void *, int, int);
void PauseMenu_ClearSpriteFlags(void *, s32, u32);
void MenuDisplay_SetPaletteOverrides(void *, int, int, int, int);
void Hud_SetGridPosition(void *, int, int);
void sub_0816CDB8(void *, int, int);
void SceneHud_GetActionIconPosition(void *, int);
void SceneHud_InitializeActionInput(struct SceneHudState *s)
{
    u8 *r, *v, *a;
    if (s->actionCategory == 0)
        sub_0816CDB8(s->actionInput, 1, 0xe1);
    else
        sub_0816CDB8(s->actionInput, 1, 0x61);
    Hud_SetGridPosition(s->actionInput, 8, 0);
    a = (u8 *)s + 0x15c0;
    r = &s->actionCursor;
    SceneHud_GetActionIconPosition(a, *r);
    PauseMenu_AddSpriteFlags(s, 5, 1);
    a = (u8 *)s + 0x1600;
    v = s->widgetResource;
    Anim_LoadKeyframe(a, v, 0x4d, 0);
    PauseMenu_AddSpriteFlags(s, 6, 1);
    Anim_LoadKeyframe((u8 *)s + 0x1660, v, 0x4e, 0);
    PauseMenu_ClearSpriteFlags(s, 4, 1);
    SceneHud_GetActionIconPosition((u8 *)s + 0x1620, *r);
    MenuDisplay_SetPaletteOverrides(s, 0, 1, 0, 0);
}
