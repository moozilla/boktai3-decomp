#include "global.h"
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_08063EC0(u8 *, u32, u32);
void Task_Kill(u8 *);
void sub_08063C60(void);
void sub_08063C88(void);
u8 *sub_08063F00(u32 a, u32 b)
{
    u8 *r = Task_Create(8, 0x144);
    if (r) {
        Task_SetCallbacks(r, (void (*)(void))sub_08063C60, (void (*)(void))sub_08063C88);
        if (sub_08063EC0(r, a, b) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
