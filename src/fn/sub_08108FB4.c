#include "global.h"
#include "engine.h"
#include "ram.h"
void CallbackTask_Update(void *);
void LinkHandshake_OnDestroy(void *);
s32 SlotMonitor_Initialize(void *, s32, u32, u32, s32, u32);
void *sub_08108FB4(s32 a, u32 b, u32 c, s32 d, u32 e)
{
    void *p;
    if (gSlotMonitor)
        return gSlotMonitor;
    p = Task_Create(2, 0x84);
    if (p) {
        Task_SetCallbacks(p, CallbackTask_Update, LinkHandshake_OnDestroy);
        if (SlotMonitor_Initialize(p, a, b, c, d, e) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
