#include "global.h"

extern u8 *gUnk_02000214;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_08178FC8(u8 *, u32);
void Task_Kill(u8 *);
void sub_08178B98(void);
void sub_08178BCC(void);

u8 *sub_0817923C(u32 a)
{
    u8 *p;
    if (gUnk_02000214 != 0)
        return gUnk_02000214;
    p = Task_Create(0xc, 0x4DA0);
    if (p != 0) {
        Task_SetCallbacks(p, sub_08178B98, sub_08178BCC);
        *(u16 *)(p + 0xA5A) = a;
        if (sub_08178FC8(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
