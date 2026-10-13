#include "global.h"
extern u8 *gUnk_02000168;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_0806A520(u8 *);
void Task_Kill(u8 *);
void sub_08069FA4(void);
void sub_0806A138(void);
u8 *sub_0806A5B4(void)
{
    u8 *r;
    if (gUnk_02000168 != 0)
        return gUnk_02000168;
    r = Task_Create(9, 0xce4);
    if (r) {
        Task_SetCallbacks(r, sub_08069FA4, sub_0806A138);
        if (sub_0806A520(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
