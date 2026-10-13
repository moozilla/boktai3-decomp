#include "global.h"
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_080506CC(u8 *, u32, u32);
void Task_Kill(u8 *);
void sub_08050694(void);
void sub_080506B8(void);
u8 *sub_080507DC(u32 a, u32 b)
{
    u8 *r = Task_Create(8, 0x3c);
    if (r) {
        Task_SetCallbacks(r, (void (*)(void))sub_08050694, (void (*)(void))sub_080506B8);
        if (sub_080506CC(r, a, b) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
