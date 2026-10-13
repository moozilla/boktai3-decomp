#include "global.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 ClockEvents_Initialize(u8 *);
void Task_Kill(u8 *);
void ClockEvents_Update(void);
void sub_08019470(void);

u8 *sub_080194DC(void)
{
    u8 *p;
    p = Task_Create(8, 0x30);
    if (p != 0) {
        Task_SetCallbacks(p, ClockEvents_Update, sub_08019470);
        if (ClockEvents_Initialize(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
