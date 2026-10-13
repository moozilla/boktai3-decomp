#include "global.h"

extern u8 *gUnk_02000190;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_0812D50C(u8 *);
void Task_Kill(u8 *);
void sub_0812D46C(void);
void sub_0812D4CC(void);

u8 *sub_0812D534(void)
{
    u8 *p;
    if (gUnk_02000190 != 0)
        return gUnk_02000190;
    p = Task_Create(0xa, 0x53c);
    if (p != 0) {
        Task_SetCallbacks(p, sub_0812D46C, sub_0812D4CC);
        if (sub_0812D50C(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
