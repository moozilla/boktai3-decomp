#include "global.h"

extern u8 *gUnk_02000158;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_0812695C(u8 *);
void Task_Kill(u8 *);
void sub_081268A4(void);
void sub_08126918(void);

u8 *sub_08126988(void)
{
    u8 *p;
    if (gUnk_02000158 != 0)
        return gUnk_02000158;
    p = Task_Create(0xa, 0x260);
    if (p != 0) {
        Task_SetCallbacks(p, sub_081268A4, sub_08126918);
        if (sub_0812695C(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
