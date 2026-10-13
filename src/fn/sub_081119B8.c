#include "global.h"
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_081117F0(u8 *);
void Task_Kill(u8 *);
void sub_081113D0(void);
void sub_081113EC(void);

u8 *sub_081119B8(void)
{
    u8 *p;
    p = Task_Create(9, 0x1ae4);
    if (p != 0) {
        Task_SetCallbacks(p, sub_081113D0, sub_081113EC);
        if (sub_081117F0(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
