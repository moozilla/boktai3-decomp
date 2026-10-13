#include "global.h"

extern u8 *gUnk_02000178;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_0806B9F0(u8 *);
void Task_Kill(u8 *);
void sub_0806B8DC(void);
void sub_0806B96C(void);

u8 *sub_0806BA00(void)
{
    u8 *p;
    if (gUnk_02000178 != 0)
        return gUnk_02000178;
    p = Task_Create(10, 0xBBC);
    if (p != 0) {
        Task_SetCallbacks(p, sub_0806B8DC, sub_0806B96C);
        if (sub_0806B9F0(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
