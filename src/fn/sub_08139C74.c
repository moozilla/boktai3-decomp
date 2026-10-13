#include "global.h"

extern u8 *gUnk_020004D0;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_08139BC0(u8 *);
void Task_Kill(u8 *);
void sub_08139B58(void);
void sub_08139BA0(void);

u8 *sub_08139C74(void)
{
    u8 *p;
    if (gUnk_020004D0 != 0)
        return gUnk_020004D0;
    p = Task_Create(0xa, 0x52c);
    if (p != 0) {
        Task_SetCallbacks(p, sub_08139B58, sub_08139BA0);
        if (sub_08139BC0(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
