#include "global.h"

extern u8 *gUnk_020004C4;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_08113340(u8 *);
void Task_Kill(u8 *);
void sub_081132E0(void);
void sub_08113310(void);

u8 *sub_0811334C(void)
{
    u8 *p;
    if (gUnk_020004C4 != 0)
        return gUnk_020004C4;
    p = Task_Create(0xa, 0x818);
    if (p != 0) {
        Task_SetCallbacks(p, sub_081132E0, sub_08113310);
        if (sub_08113340(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
