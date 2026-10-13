#include "global.h"

extern u8 *gUnk_020004CC;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_0813930C(u8 *);
void Task_Kill(u8 *);
void sub_08139298(void);
void sub_081392DC(void);

u8 *sub_081393DC(void)
{
    u8 *p;
    if (gUnk_020004CC != 0)
        return gUnk_020004CC;
    p = Task_Create(0xa, 0x39c);
    if (p != 0) {
        Task_SetCallbacks(p, sub_08139298, sub_081392DC);
        if (sub_0813930C(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
