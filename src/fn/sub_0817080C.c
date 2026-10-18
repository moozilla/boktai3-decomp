#define SceneHud_InitializeActionInput sub_0817080C
#include "global.h"
#include "scene_hud_display.h"
void PauseMenu_AddSpriteFlags(void *, int, int);
void PauseMenu_ClearSpriteFlags(void *, s32, u32);
void sub_081663A0(void *, int, int, int, int);
void Hud_SetGridPosition(void *, int, int);
void sub_0816CDB8(void *, int, int);
void sub_0816D920(void *, int);
void sub_0817080C(struct SceneHudState *s)
{
    u8 *r, *v, *a;
    if (s->actionCategory == 0)
        sub_0816CDB8(s->actionInput, 1, 0xe1);
    else
        sub_0816CDB8(s->actionInput, 1, 0x61);
    Hud_SetGridPosition(s->actionInput, 8, 0);
    a = (u8 *)s + 0x15c0;
    r = &s->actionCursor;
    sub_0816D920(a, *r);
    PauseMenu_AddSpriteFlags(s, 5, 1);
    a = (u8 *)s + 0x1600;
    v = s->widgetResource;
    Anim_LoadKeyframe(a, v, 0x4d, 0);
    PauseMenu_AddSpriteFlags(s, 6, 1);
    Anim_LoadKeyframe((u8 *)s + 0x1660, v, 0x4e, 0);
    PauseMenu_ClearSpriteFlags(s, 4, 1);
    sub_0816D920((u8 *)s + 0x1620, *r);
    sub_081663A0(s, 0, 1, 0, 0);
}
