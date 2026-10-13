#include "global.h"
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_0804F5F4(u8 *, u32, u32);
void Task_Kill(u8 *);
void sub_0804F3D8(void);
void sub_0804F418(void);
u8 *sub_0804F67C(u32 a, u32 b)
{
    u8 *r = Task_Create(8, 0x268);
    if (r) {
        Task_SetCallbacks(r, (void (*)(void))sub_0804F3D8, (void (*)(void))sub_0804F418);
        if (sub_0804F5F4(r, a, b) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
