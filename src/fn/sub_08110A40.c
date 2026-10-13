#include "global.h"

extern u8 *gUnk_020001D4;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_08110894(u8 *);
void Task_Kill(u8 *);
void sub_08110834(void);
void sub_0811086C(void);

u8 *sub_08110A40(void)
{
    u8 *p;
    if (gUnk_020001D4 != 0)
        return gUnk_020001D4;
    p = Task_Create(0xb, 0x1034);
    if (p != 0) {
        Task_SetCallbacks(p, sub_08110834, sub_0811086C);
        if (sub_08110894(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
