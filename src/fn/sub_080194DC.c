#include "global.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 ClockEvents_Initialize(u8 *);
void Task_Kill(u8 *);
void ClockEvents_Update(void);
void ClockEvents_Destroy(void);

u8 *ClockEvents_Create(void)
{
    u8 *p;
    p = Task_Create(8, 0x30);
    if (p != 0) {
        Task_SetCallbacks(p, ClockEvents_Update, ClockEvents_Destroy);
        if (ClockEvents_Initialize(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
