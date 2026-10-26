#include "global.h"
#include "ram.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 DirectionalProjectile_InitPool(u8 *);
void Task_Kill(u8 *);
void SpritePool12_UpdateActive(void);
void DirectionalProjectile_DestroyPool(void);

u8 *DirectionalProjectile_EnsurePool(void)
{
    u8 *p;
    if (gMovingSpritePool != 0)
        return gMovingSpritePool;
    p = Task_Create(0xa, 0x7bc);
    if (p != 0) {
        Task_SetCallbacks(p, SpritePool12_UpdateActive, DirectionalProjectile_DestroyPool);
        if (DirectionalProjectile_InitPool(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
