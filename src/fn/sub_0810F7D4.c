#include "global.h"

extern u8 *gUnk_020001D0;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_0810F678(u8 *);
void Task_Kill(u8 *);
void sub_0810F604(void);
void sub_0810F63C(void);

u8 *sub_0810F7D4(void)
{
    u8 *p;
    if (gUnk_020001D0 != 0)
        return gUnk_020001D0;
    p = Task_Create(0xb, 0x1504);
    if (p != 0) {
        Task_SetCallbacks(p, sub_0810F604, sub_0810F63C);
        if (sub_0810F678(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
