#include "global.h"

extern u8 *gUnk_02000148;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_08125B00(u8 *);
void Task_Kill(u8 *);
void sub_08125A94(void);
void sub_08125AF4(void);

u8 *sub_08125B0C(void)
{
    u8 *p;
    if (gUnk_02000148 != 0)
        return gUnk_02000148;
    p = Task_Create(0xa, 0x1e4);
    if (p != 0) {
        Task_SetCallbacks(p, sub_08125A94, sub_08125AF4);
        if (sub_08125B00(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
