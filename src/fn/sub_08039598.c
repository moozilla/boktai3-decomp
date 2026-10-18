#include "global.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 WirelessExit_Init(u8 *);
void Task_Kill(u8 *);
void WirelessExit_Update(void);
void WirelessExit_OnDestroy(void);

u8 *WirelessExit_Create(void)
{
    u8 *p;
    p = Task_Create(3, 0x20);
    if (p != 0) {
        Task_SetCallbacks(p, WirelessExit_Update, WirelessExit_OnDestroy);
        if (WirelessExit_Init(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
