#include "global.h"
#include "callback_pool.h"
#include "ram.h"
void Sprite_Remove(void *);
void Collider_Remove(void *);
void ScaledShadow_Destroy(void *);
static inline u8 CallbackPool_IsActive(u32 *mask, s32 i)
{
    if (*mask & (1 << i))
        return TRUE;
    return FALSE;
}
s32 CallbackPool_DestroyEntries(struct CallbackPool *pool)
{
    s32 i;
    for (i = 0; i < 4; i++) {
        if (CallbackPool_IsActive(&pool->activeMask, i)) {
            Sprite_Remove(&pool->entries[i]);
            Collider_Remove((u8 *)&pool->entries[i] + 0x58);
            if (pool->entries[i].effectIndex != -1) {
                ScaledShadow_Destroy((u8 *)&pool->entries[i] + 0xac);
                pool->entries[i].effectIndex = -1;
            }
        }
    }
    gActorGroupPool = 0;
    return 0;
}
