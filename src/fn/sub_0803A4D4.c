#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 WirelessGameItems_Init(void *);
void Task_Kill(void *);
void WirelessGameItems_Update(void);
void WirelessGameItems_Destroy(void);
void *WirelessGameItems_Create(void)
{
    void *p = Task_Create(11, 0xb5c);
    if (p) {
        Task_SetCallbacks(p, WirelessGameItems_Update, WirelessGameItems_Destroy);
        if (WirelessGameItems_Init(p) >= 0)
            goto ok;
        Task_Kill(p);
        return 0;
    }
ok:
    return p;
}
