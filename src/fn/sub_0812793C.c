#include "global.h"
#include "ram.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 SpriteProjectile_InitPool(u8 *);
void Task_Kill(u8 *);
void StateSlotPool_Update(void);
void SpriteProjectile_DestroyPool(void);

u8 *SpriteProjectile_EnsurePool(void)
{
    u8 *p;
    if (gPooledSpriteManager != 0)
        return gPooledSpriteManager;
    p = Task_Create(0x8, 0x864);
    if (p != 0) {
        Task_SetCallbacks(p, StateSlotPool_Update, SpriteProjectile_DestroyPool);
        if (SpriteProjectile_InitPool(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
