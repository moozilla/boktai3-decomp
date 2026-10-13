#include "global.h"
extern u8 *gUnk_02000488;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_0804D500(u8 *, u32, u32);
void Task_Kill(u8 *);
void sub_0804D378(void);
void sub_0804D4B0(void);
u8 *sub_0804D5CC(u32 a, u32 b)
{
    u8 *r;
    if (gUnk_02000488 != 0)
        return gUnk_02000488;
    r = Task_Create(9, 0x484);
    if (r) {
        Task_SetCallbacks(r, sub_0804D378, sub_0804D4B0);
        if (sub_0804D500(r, a, b) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
