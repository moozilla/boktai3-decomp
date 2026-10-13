#include "global.h"

extern u8 *gUnk_02000200;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_0811B714(u8 *);
void Task_Kill(u8 *);
void sub_0811B5F0(void);
void sub_0811B668(void);

u8 *sub_0811B72C(void)
{
    u8 *p;
    if (gUnk_02000200 != 0)
        return gUnk_02000200;
    p = Task_Create(5, 0xc0);
    if (p != 0) {
        Task_SetCallbacks(p, sub_0811B5F0, sub_0811B668);
        if (sub_0811B714(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
