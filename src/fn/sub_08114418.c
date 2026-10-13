#include "global.h"

extern u8 *gUnk_020004C8;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_081143D8(u8 *);
void Task_Kill(u8 *);
void sub_08114370(void);
void sub_081143B8(void);

u8 *sub_08114418(void)
{
    u8 *p;
    if (gUnk_020004C8 != 0)
        return gUnk_020004C8;
    p = Task_Create(0xa, 0xd9c);
    if (p != 0) {
        Task_SetCallbacks(p, sub_08114370, sub_081143B8);
        if (sub_081143D8(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
