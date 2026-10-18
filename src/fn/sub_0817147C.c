#include "global.h"
void sub_08170D78(void *);
void Hud_RestoreSecondaryGridSelection(void *);
void sub_08170B48(void *, int);
void PauseMenu_ClearSpriteFlags(void *, int, int);
void PauseMenu_AddSpriteFlags(void *, int, int);
void sub_08166CD0(void *);
void sub_08166534(void *, int, int);
void sub_08166ED4(void *, int, int, int, int);
void sub_08166F84(void *, int, int, int, int);
void sub_081672FC(void *);
void PauseInventory_UpdateConsumableActions(void);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void MessageWindow_Clear(void);
void sub_081663A0(void *, int, int, int, int);
void sub_08170F54(void *);
void sub_08166B7C(void *, int);
void PauseInventory_ReturnToConsumableActions(u8 *p)
{
    u8 *q;
    int z;
    sub_08170D78(p);
    q = p + 0x4384;
    Hud_RestoreSecondaryGridSelection(q);
    sub_08170B48(q, 0xe1);
    PauseMenu_ClearSpriteFlags(p, 4, 1);
    PauseMenu_AddSpriteFlags(p, 6, 1);
    PauseMenu_AddSpriteFlags(p, 5, 1);
    sub_08166CD0(p);
    sub_08166534(p, 10, 6);
    z = 0;
    sub_08166ED4(p, 8, 3, 0, z);
    sub_08166F84(p, 8, 4, 0, z);
    sub_081672FC(p);
    Hud_SetInteractionCallback(p, PauseInventory_UpdateConsumableActions, 0);
    MessageWindow_Clear();
    sub_081663A0(p, 0, 1, 0, z);
    sub_08170F54(p);
    sub_08166B7C(p, 3);
}
