#include "global.h"
extern u8 *gUnk_02000128;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_08067900(u8 *);
void Task_Kill(u8 *);
void sub_08067854(void);
void sub_080678B0(void);
u8 *sub_08067938(void)
{
    u8 *r;
    if (gUnk_02000128 != 0)
        return gUnk_02000128;
    r = Task_Create(10, 0x59c);
    if (r) {
        Task_SetCallbacks(r, sub_08067854, sub_080678B0);
        if (sub_08067900(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
