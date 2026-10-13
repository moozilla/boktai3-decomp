#include "global.h"

extern u8 *gUnk_020001BC;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_08108A1C(u8 *);
void Task_Kill(u8 *);
void sub_0810896C(void);
void sub_081089D8(void);

u8 *sub_08108A4C(void)
{
    u8 *p;
    if (gUnk_020001BC != 0)
        return gUnk_020001BC;
    p = Task_Create(0x8, 0x158);
    if (p != 0) {
        Task_SetCallbacks(p, sub_0810896C, sub_081089D8);
        if (sub_08108A1C(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
