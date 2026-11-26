#include "scene/actor/route_sprite_actor.h"
#include "engine/ram.h"
extern u32 gUnk_02000560;
static inline u8 Occupied(u32 *mask, s32 index)
{
    if (*mask & (1 << index))
        return TRUE;
    return FALSE;
}
static inline u32 Paused(void)
{
    u32 mask = 0x400;
    return (gEngineFlags | gScenePauseFlags) & mask;
}
static inline u32 Locked(void)
{
    u32 mask = 4;
    return gUnk_02000560 & mask;
}
s32 RouteSpritePool_Tick(struct RouteSpritePool *pool)
{
    struct RouteSpriteActor *start = pool->slots;
    if (pool->occupiedMask) {
        if (!Paused()) {
            if (!Locked()) {
                s32 i = 0;
                RouteSpriteStateCallback *table = gUnk_08605BB0;
                for (; i < 4; i++) {
                    if (Occupied(&pool->occupiedMask, i))
                        table[start[i].state](pool, &start[i]);
                }
            }
        }
    }
    return 0;
}
