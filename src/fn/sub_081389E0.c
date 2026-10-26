#include "global.h"
#include "ram.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 PlayerSlotDisplay_Initialize(u8 *);
void Task_Kill(u8 *);
void PositionEffectDisplay_UpdateSources(void);
void PositionEffectDisplay_Destroy(void);

u8 *PositionEffectDisplay_Create(void)
{
    u8 *p;
    if (gSpriteSlots != 0)
        return gSpriteSlots;
    p = Task_Create(0xa, 0x19c);
    if (p != 0) {
        Task_SetCallbacks(p, PositionEffectDisplay_UpdateSources, PositionEffectDisplay_Destroy);
        if (PlayerSlotDisplay_Initialize(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
