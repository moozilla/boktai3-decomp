#include "global.h"
extern u8 *gUnk_02000140;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_08122D58(u8 *);
void Task_Kill(u8 *);
void sub_081228F0(void);
void sub_0812286C(void);
void sub_08122DA0(void)
{
    u8 *p;
    if (gUnk_02000140 == 0) {
        p = Task_Create(10, 0x550);
        if (p != 0) {
            Task_SetCallbacks(p, sub_081228F0, sub_0812286C);
            if (sub_08122D58(p) < 0)
                Task_Kill(p);
        }
    }
}
