#include "global.h"
#include "ram.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 WoodenCrate_InitManager(u8 *);
void Task_Kill(u8 *);
void SlotPool_UpdateActiveSlots(void);
void WoodenCrate_DestroyManager(void);

u8 *WoodenCrate_GetOrCreateManager(void)
{
    u8 *p;
    if (gWoodenCrateManager != 0)
        return gWoodenCrateManager;
    p = Task_Create(9, 0xa40);
    if (p != 0) {
        Task_SetCallbacks(p, SlotPool_UpdateActiveSlots, WoodenCrate_DestroyManager);
        if (WoodenCrate_InitManager(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
