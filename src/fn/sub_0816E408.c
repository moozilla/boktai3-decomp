#include "global.h"
s32 sub_08165D10(void *);
void MenuTransition_UpdateTimedMessage(void);
void Hud_SetInteractionCallback(void *, void (*)(void), u32);
void PauseMenu_OnItemUseDelayUpdate(u8 *s)
{
    u16 *counter = (u16 *)(s + 0xA4E);
    if (*counter <= 0xF) {
        (*counter)++;
    } else if (sub_08165D10(s) != 0) {
        Hud_SetInteractionCallback(s, MenuTransition_UpdateTimedMessage, 1);
    }
}
