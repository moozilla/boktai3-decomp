#include "global.h"

extern u8 *gUnk_020000E0;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_0803336C(u8 *);
void Task_Kill(u8 *);
void sub_080332B0(void);
void sub_08033340(void);

u8 *sub_080333F0(void)
{
    u8 *p;
    if (gUnk_020000E0 != 0)
        return gUnk_020000E0;
    p = Task_Create(0x3, 0x19c);
    if (p != 0) {
        Task_SetCallbacks(p, sub_080332B0, sub_08033340);
        if (sub_0803336C(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
