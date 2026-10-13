#include "global.h"
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_08056A00(u8 *);
void Task_Kill(u8 *);
void sub_08056954(void);
void sub_080569D8(void);
u8 *sub_08056A20(void)
{
    u8 *r = Task_Create(9, 0x2c8);
    if (r) {
        Task_SetCallbacks(r, sub_08056954, sub_080569D8);
        if (sub_08056A00(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
