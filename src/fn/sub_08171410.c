#include "global.h"
void sub_08170D78(void *);
void sub_08171D5C(void);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void sub_0816CDB8(void *, int, int);
void Hud_SetGridPosition(void *, int, int);
void PauseMenu_ClearSpriteFlags(void *, int, int);
void PauseInventory_RefreshConsumableDescription(void *);
void MenuDisplay_SetPaletteOverrides(void *, int, int, int, int);
void MenuDisplay_UpdateActionPrompt(void *, int);
void PauseInventory_BeginConsumableDiscardSelection(u8 *p, int flag)
{
    u8 *q;
    sub_08170D78(p);
    Hud_SetInteractionCallback(p, sub_08171D5C, 1);
    q = p + 0x4384;
    sub_0816CDB8(q, 1, 0);
    if (flag)
        Hud_SetGridPosition(q, 0, 0);
    PauseMenu_ClearSpriteFlags(p, 5, 1);
    PauseInventory_RefreshConsumableDescription(p);
    MenuDisplay_SetPaletteOverrides(p, 1, 0, 0, 0);
    MenuDisplay_UpdateActionPrompt(p, 3);
}
