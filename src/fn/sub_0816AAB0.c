#include "global.h"
#define MenuPrompt_WaitForChildPhase sub_0816AAB0
struct MenuPrompt {
    u8 unk0[0x48F4];
    void *child;
};
s32 sub_081BD914(void *);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void sub_0816AD34(struct MenuPrompt *);
void sub_0816AAB0(struct MenuPrompt *p)
{
    if (sub_081BD914(p->child)) {
        ScreenTransition_SendCommand(2, 4, 4, 4, 4, 65535, 0);
        sub_0816AD34(p);
    }
}
