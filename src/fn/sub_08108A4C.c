#include "global.h"
#include "ram.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 AnimatedTile_InitManager(u8 *);
void Task_Kill(u8 *);
void CallbackSlots_DispatchActive(void);
void AnimatedTile_OnManagerDestroy(void);

u8 *AnimatedTile_CreateManager(void)
{
    u8 *p;
    if (gNamedEntryPool != 0)
        return gNamedEntryPool;
    p = Task_Create(0x8, 0x158);
    if (p != 0) {
        Task_SetCallbacks(p, CallbackSlots_DispatchActive, AnimatedTile_OnManagerDestroy);
        if (AnimatedTile_InitManager(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
