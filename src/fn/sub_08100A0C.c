#include "global.h"

extern u8 *gUnk_020001A8;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_081009F4(u8 *);
void Task_Kill(u8 *);
void sub_08100938(void);
void sub_081009B0(void);

u8 *sub_08100A0C(void)
{
    u8 *p;
    if (gUnk_020001A8 != 0)
        return gUnk_020001A8;
    p = Task_Create(8, 0x49c);
    if (p != 0) {
        Task_SetCallbacks(p, sub_08100938, sub_081009B0);
        if (sub_081009F4(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
