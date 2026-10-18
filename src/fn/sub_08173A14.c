#include "global.h"
void sub_0817373C(void *, int);
void sub_081740CC(void);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void sub_081663A0(void *, int, int, int, int);
void PauseMenu_ClearSpriteFlags(void *, int, int);
void PauseMenu_ResetItemCursor(void *);
void sub_0816CDB8(void *, int, int);
void sub_08166B7C(void *, int);
void sub_08173470(void *);
void PauseInventory_BeginItemDiscardSelection(u8 *p, int flag)
{
    sub_0817373C(p, 1);
    Hud_SetInteractionCallback(p, sub_081740CC, 1);
    sub_081663A0(p, 1, 0, 0, 0);
    PauseMenu_ClearSpriteFlags(p, 5, 1);
    if (flag)
        PauseMenu_ResetItemCursor(p);
    sub_0816CDB8(p + 0x4384, 1, 0);
    sub_08166B7C(p, 2);
    sub_08173470(p);
}
