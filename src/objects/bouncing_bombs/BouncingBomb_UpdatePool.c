#include "objects/bouncing_bombs/bouncing_object.h"
#include "engine/engine_pause.h"
static inline u8 Occupied(struct BouncingPool *pool, s32 i)
{
    if (pool->used & (1 << i))
        return TRUE;
    return FALSE;
}

s32 BouncingBomb_UpdatePool(struct BouncingPool *pool)
{
    struct BouncingObject *slots = pool->slots;
    s32 i;

    if (pool->used == 0) {
        Task_Kill(pool);
        pool->stopping = 1;
    } else if (!Engine_PauseBits(0x400)) {
        u32 mask = 4;
        if (!(gUnk_02000560 & mask)) {
            for (i = 0; i < 12; i++) {
                if (Occupied(pool, i))
                    gBouncingObjectStateCallbacks[slots[i].state](pool, &slots[i]);
            }
        }
    }
    return 0;
}
