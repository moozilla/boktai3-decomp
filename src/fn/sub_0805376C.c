#include "global.h"
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_080532B0(u8 *, u32, u32);
void Task_Kill(u8 *);
void sub_08053178(void);
void sub_080531BC(void);
u8 *sub_0805376C(u32 a, u32 b)
{
    u8 *r = Task_Create(9, 0x388);
    if (r) {
        Task_SetCallbacks(r, (void (*)(void))sub_08053178, (void (*)(void))sub_080531BC);
        if (sub_080532B0(r, a, b) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
