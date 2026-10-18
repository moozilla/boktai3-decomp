#include "global.h"
void sub_0817373C(void *, int);
void sub_08173D2C(void);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void Hud_SetGridPosition(void *, int, int);
void sub_081663A0(void *, int, int, int, int);
void PauseMenu_AddSpriteFlags(void *, int, int);
void sub_0816CDB8(void *, int, int);
void sub_08166B7C(void *, int);
void sub_08173470(void *);
void PauseInventory_BeginItemSwapSelection(u8 *p, int flag)
{
    sub_0817373C(p, 0);
    Hud_SetInteractionCallback(p, sub_08173D2C, 1);
    if (flag)
        Hud_SetGridPosition(p + 0x4384, 0, 0);
    sub_081663A0(p, 1, 0, 0, 0);
    PauseMenu_AddSpriteFlags(p, 6, 1);
    sub_0816CDB8(p + 0x4384, 1, 0);
    sub_08166B7C(p, 2);
    sub_08173470(p);
}
