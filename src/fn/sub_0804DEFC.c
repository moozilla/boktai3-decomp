#include "global.h"
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_0804DD48(u8 *, u32);
void Task_Kill(u8 *);
void sub_0804DC40(void);
u32 sub_0804DD10(u8 *);
u8 *sub_0804DEFC(u32 a)
{
    u8 *r = Task_Create(8, 0x190);
    if (r) {
        Task_SetCallbacks(r, sub_0804DC40, (void (*)(void))sub_0804DD10);
        if (sub_0804DD48(r, a) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
