#include "global.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_08057714(u8 *);
void Task_Kill(u8 *);
void sub_080576C8(void);
void sub_08057704(void);

u8 *sub_08057924(void)
{
    u8 *p;
    p = Task_Create(9, 0x84);
    if (p != 0) {
        Task_SetCallbacks(p, sub_080576C8, sub_08057704);
        if (sub_08057714(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
