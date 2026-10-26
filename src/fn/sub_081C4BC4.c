#include "global.h"
#include "ram.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void *), void (*)(void *));
s32 sub_081C49F4(void *, void *);
void Task_Kill(void *);
void ShipWheel_Update(void *);
void ShipWheel_OnDestroy(void *);
void *ShipWheel_Create(void *a)
{
    void *p;
    if (gShipWheel != 0)
        return gShipWheel;
    p = Task_Create(0xb, 0xe4);
    if (p != 0) {
        Task_SetCallbacks(p, ShipWheel_Update, ShipWheel_OnDestroy);
        if (sub_081C49F4(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    gShipWheel = p;
    return p;
}
