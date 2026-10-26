#include "route_sprite_actor.h"
extern u32 gUnk_030053F0, gUnk_02000560;
static inline u8 Occupied(u32 *mask, s32 index)
{
    if (*mask & (1 << index))
        return TRUE;
    return FALSE;
}
static inline u32 Paused(void)
{
    u32 mask = 0x400;
    return (gEngineFlags | gUnk_030053F0) & mask;
}
static inline u32 Locked(void)
{
    u32 mask = 4;
    return gUnk_02000560 & mask;
}
s32 RouteSpritePool_Tick(struct RouteSpritePool *pool)
{
    struct RouteSpriteActor *start = pool->slots;
    struct RouteSpriteActor *slot;
    /* Keep the post-loop state stride as an integer address; its final value
     * is unused and may extend beyond the complete pool object. */
    u32 state;
    RouteSpriteStateCallback *table;
    s32 i;
    if (pool->occupiedMask) {
        if (!Paused()) {
            if (!Locked()) {
                i = 0;
                do {
                    table = gUnk_08605BB0;
                    state = (u32)&pool->slots[0].state;
                    slot = start;
                } while (0);
                for (; i < 4;) {
                    if (Occupied(&pool->occupiedMask, i))
                        sub_08249240(pool, slot, (void *)table[*(u8 *)state]);
                    {
                        u32 stride = 0x178;
                        state += stride;
                        slot = (void *)((u32)slot + stride);
                    }
                    i++;
                }
            }
        }
    }
    return 0;
}
