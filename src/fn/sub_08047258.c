#include "global.h"

extern u8 *gUnk_02000074;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_080471CC(u8 *);
void Task_Kill(u8 *);
void sub_08047168(void);
void sub_080471A8(void);

u8 *sub_08047258(void)
{
    u8 *p;
    if (gUnk_02000074 != 0)
        return gUnk_02000074;
    p = Task_Create(0x9, 0x2f0);
    if (p != 0) {
        Task_SetCallbacks(p, sub_08047168, sub_080471A8);
        if (sub_080471CC(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
