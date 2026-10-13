#include "global.h"

extern u8 *gUnk_020004C0;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_08138924(u8 *);
void Task_Kill(u8 *);
void sub_08138778(void);
void sub_081388F4(void);

u8 *sub_081389E0(void)
{
    u8 *p;
    if (gUnk_020004C0 != 0)
        return gUnk_020004C0;
    p = Task_Create(0xa, 0x19c);
    if (p != 0) {
        Task_SetCallbacks(p, sub_08138778, sub_081388F4);
        if (sub_08138924(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
