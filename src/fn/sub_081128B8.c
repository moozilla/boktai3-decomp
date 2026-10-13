#include "global.h"
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_08112758(u8 *);
void Task_Kill(u8 *);
void sub_08112714(void);
void sub_08112734(void);
u8 *sub_081128B8(void)
{
    u8 *p = Task_Create(0xb, 0x1214);
    if (p != 0) {
        Task_SetCallbacks(p, sub_08112714, sub_08112734);
        if (sub_08112758(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
