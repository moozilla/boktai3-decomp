#include "global.h"

extern u8 *gUnk_02000154;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_081265DC(u8 *);
void Task_Kill(u8 *);
void sub_08126568(void);
void sub_081265A4(void);

u8 *sub_08126604(void)
{
    u8 *p;
    if (gUnk_02000154 != 0)
        return gUnk_02000154;
    p = Task_Create(0xa, 0x151c);
    if (p != 0) {
        Task_SetCallbacks(p, sub_08126568, sub_081265A4);
        if (sub_081265DC(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
