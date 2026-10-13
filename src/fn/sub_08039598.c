#include "global.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_08039568(u8 *);
void Task_Kill(u8 *);
void sub_080394D4(void);
void sub_08039548(void);

u8 *sub_08039598(void)
{
    u8 *p;
    p = Task_Create(3, 0x20);
    if (p != 0) {
        Task_SetCallbacks(p, sub_080394D4, sub_08039548);
        if (sub_08039568(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
