#include "global.h"

extern u8 *gUnk_0200016C;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_08128124(u8 *);
void Task_Kill(u8 *);
void sub_0812808C(void);
void sub_081280E4(void);

u8 *sub_0812815C(void)
{
    u8 *p;
    if (gUnk_0200016C != 0)
        return gUnk_0200016C;
    p = Task_Create(0xa, 0x7bc);
    if (p != 0) {
        Task_SetCallbacks(p, sub_0812808C, sub_081280E4);
        if (sub_08128124(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
