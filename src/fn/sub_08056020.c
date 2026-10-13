#include "global.h"

extern u8 *gUnk_0200011C;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_08055FA4(u8 *);
void Task_Kill(u8 *);
void sub_08055D74(void);
void sub_08055DA0(void);

u8 *sub_08056020(void)
{
    u8 *p;
    if (gUnk_0200011C != 0)
        return gUnk_0200011C;
    p = Task_Create(0x4, 0x1d4);
    if (p != 0) {
        Task_SetCallbacks(p, sub_08055D74, sub_08055DA0);
        if (sub_08055FA4(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
