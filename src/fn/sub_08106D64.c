#include "global.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_08106D20(u8 *);
void Task_Kill(u8 *);
void sub_08106BCC(void);
void sub_08106C88(void);

u8 *sub_08106D64(void)
{
    u8 *p;
    p = Task_Create(10, 0xA8);
    if (p != 0) {
        Task_SetCallbacks(p, sub_08106BCC, sub_08106C88);
        if (sub_08106D20(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
