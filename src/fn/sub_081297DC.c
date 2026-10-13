#include "global.h"

extern u8 *gUnk_02000174;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_081297A4(u8 *);
void Task_Kill(u8 *);
void sub_081296D0(void);
void sub_0812975C(void);

u8 *sub_081297DC(void)
{
    u8 *p;
    if (gUnk_02000174 != 0)
        return gUnk_02000174;
    p = Task_Create(0x8, 0xcbc);
    if (p != 0) {
        Task_SetCallbacks(p, sub_081296D0, sub_0812975C);
        if (sub_081297A4(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
