#include "global.h"
extern u8 *gUnk_02000124;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_08066C0C(u8 *);
void Task_Kill(u8 *);
void sub_08066AE0(void);
void sub_08066B28(void);
u8 *sub_08066C34(void)
{
    u8 *r;
    if (gUnk_02000124 != 0)
        return gUnk_02000124;
    r = Task_Create(9, 0xa20);
    if (r) {
        Task_SetCallbacks(r, sub_08066AE0, sub_08066B28);
        if (sub_08066C0C(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
