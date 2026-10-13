#include "global.h"

extern u8 *gUnk_020004D8;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_0813A7A8(u8 *);
void Task_Kill(u8 *);
void sub_0813A620(void);
void sub_0813A788(void);

u8 *sub_0813A860(void)
{
    u8 *p;
    if (gUnk_020004D8 != 0)
        return gUnk_020004D8;
    p = Task_Create(0xa, 0xe4);
    if (p != 0) {
        Task_SetCallbacks(p, sub_0813A620, sub_0813A788);
        if (sub_0813A7A8(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
