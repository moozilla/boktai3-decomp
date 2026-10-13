#include "global.h"
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_08056E0C(u8 *);
void Task_Kill(u8 *);
void sub_08056DBC(void);
void sub_08056DFC(void);
u8 *sub_08056EE4(void)
{
    u8 *p = Task_Create(9, 0xb0);
    if (p != 0) {
        Task_SetCallbacks(p, sub_08056DBC, sub_08056DFC);
        if (sub_08056E0C(p) != 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
