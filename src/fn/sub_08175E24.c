#define SelectionPanel_ConfirmSettings sub_08175D34
#define SelectionPanel_WaitConfirmDelay sub_08175E24
#define MenuPanel_SetCallback Hud_SetInteractionCallback
// Provisional panel family; field meanings follow static consumers.
#include "global.h"
struct SelectionPanel { u8 unk0[0xA4E]; u16 timer; };
void sub_08175854(u32);
void sub_081660D0(void *, s32, s32);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void sub_08175D34(void);
void sub_08175E24(struct SelectionPanel *p)
{
    if (++p->timer > 59) {
        sub_08175854(5);
        sub_081660D0(p, 1, -1);
        Hud_SetInteractionCallback(p, sub_08175D34, 0);
    }
}
