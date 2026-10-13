#include "global.h"

extern u8 *gUnk_02000180;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_0812B2CC(u8 *);
void Task_Kill(u8 *);
void sub_0812B1F4(void);
void sub_0812B27C(void);

u8 *sub_0812B2F8(void)
{
    u8 *p;
    if (gUnk_02000180 != 0)
        return gUnk_02000180;
    p = Task_Create(0x8, 0xc80);
    if (p != 0) {
        Task_SetCallbacks(p, sub_0812B1F4, sub_0812B27C);
        if (sub_0812B2CC(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
