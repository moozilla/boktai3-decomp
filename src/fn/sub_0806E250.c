#include "global.h"

extern u8 *gUnk_02000198;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_0806E224(u8 *);
void Task_Kill(u8 *);
void sub_0806E130(void);
void sub_0806E1C0(void);

u8 *sub_0806E250(void)
{
    u8 *p;
    if (gUnk_02000198 != 0)
        return gUnk_02000198;
    p = Task_Create(10, 0x600);
    if (p != 0) {
        Task_SetCallbacks(p, sub_0806E130, sub_0806E1C0);
        if (sub_0806E224(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
