#include "global.h"
void sub_08173508(void *, int);
void Hud_SetGridPosition(void *, int, int);
void sub_0816D920(void *, int);
void PauseMenu_AddSpriteFlags(void *, int, int);
void PauseMenu_ClearSpriteFlags(void *, int, int);
void Anim_LoadKeyframe(void *, void *, int, int);
void PauseInventory_InitItemCursors(u8 *p)
{
    u8 *q = p + 0x4384, *r, *v, *a;
    sub_08173508(q, 0xe1);
    Hud_SetGridPosition(q, 8, 0);
    a = p + 0x15c0;
    r = p + 0x4387;
    sub_0816D920(a, *r);
    PauseMenu_AddSpriteFlags(p, 5, 1);
    a = p + 0x1600;
    v = p + 0x64;
    Anim_LoadKeyframe(a, v, 0x4d, 0);
    PauseMenu_AddSpriteFlags(p, 6, 1);
    Anim_LoadKeyframe(p + 0x1660, v, 0x4e, 0);
    PauseMenu_ClearSpriteFlags(p, 4, 1);
    sub_0816D920(p + 0x1620, *r);
}
