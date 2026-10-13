#include "global.h"

extern u8 *gUnk_0200017C;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_0812A4C0(u8 *);
void Task_Kill(u8 *);
void sub_0812A418(void);
void sub_0812A480(void);

u8 *sub_0812A500(void)
{
    u8 *p;
    if (gUnk_0200017C != 0)
        return gUnk_0200017C;
    p = Task_Create(0x8, 0xc10);
    if (p != 0) {
        Task_SetCallbacks(p, sub_0812A418, sub_0812A480);
        if (sub_0812A4C0(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
