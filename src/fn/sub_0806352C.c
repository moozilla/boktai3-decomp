#include "global.h"
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_08063334(u8 *);
void Task_Kill(u8 *);
void sub_080632AC(void);
void sub_080632EC(void);
u8 *sub_0806352C(void)
{
    u8 *r = Task_Create(0xB, 0x1D24);
    if (r) {
        Task_SetCallbacks(r, sub_080632AC, sub_080632EC);
        r[0x1AA7] = 2;
        r[0x1AA8] = 2;
        if (sub_08063334(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
