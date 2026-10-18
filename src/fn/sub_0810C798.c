#define SlotMenu_UpdateExitTransition sub_0810C798
#define SlotMenu_Destroy              sub_0810C724
#include "global.h"
struct SlotMenu {
    u8 unknown0[0xF6C];
    u8 transitionPending;
    u8 unknownF6D;
    u16 transitionTimer;
};
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void sub_0810C724(struct SlotMenu *);
void sub_0810C798(struct SlotMenu *p)
{
    if (p->transitionPending) {
        ScreenTransition_SendCommand(3, 4, 4, 4, 4, 0xFFFF, 0);
        p->transitionTimer = 0;
        p->transitionPending = 0;
    }
    if (p->transitionTimer == 10)
        SlotMenu_Destroy(p);
    p->transitionTimer++;
}
