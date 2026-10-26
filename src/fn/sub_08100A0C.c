#include "global.h"
#include "ram.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 FloorSwitch_InitManager(u8 *);
void Task_Kill(u8 *);
void SpriteSlots_UpdateActive(void);
void FloorSwitch_OnManagerDestroy(void);

u8 *FloorSwitch_GetOrCreateManager(void)
{
    u8 *p;
    if (gFloorSwitchManager != 0)
        return gFloorSwitchManager;
    p = Task_Create(8, 0x49c);
    if (p != 0) {
        Task_SetCallbacks(p, SpriteSlots_UpdateActive, FloorSwitch_OnManagerDestroy);
        if (FloorSwitch_InitManager(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
