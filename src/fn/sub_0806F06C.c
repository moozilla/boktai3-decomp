// MATCHING_CONCESSION: register-pinning
/* SceneActorQuery_FindInBox: like sub_0806EFB8, but skips actors with flag 0x400 before
   advancing the cursor and tests a center +/- radius box on all three axes.
   The loop counter is pinned to r5 (rasolve: needs priority above the three pointers). */
#include "scene_actor_registry.h"
struct SceneActorRegistryActor *SceneActorRegistry_FindAvailableAfterId(u32);
static inline u8 Inactive(struct SceneActorRegistryActor *a)
{
    {
        u32 mask = 0x40;
        if (a->flags2D4 & mask)
            return 1;
    }
    {
        u32 mask = 0x20000;
        if (a->flags2D0 & mask)
            return 1;
    }
    if (a->health <= 0)
        return 1;
    return 0;
}
struct SceneActorRegistryActor *sub_0806F06C(const struct ContactPosition *center,
                                             const struct ContactPosition *radius, u32 *cursor)
{
    s32 i = 0;
    do {
        struct SceneActorRegistryActor *a = SceneActorRegistry_FindAvailableAfterId(*cursor);
        if (!a) {
            return 0;
        }
        {
            u32 mask = 0x400;
            if (a->flags2D0 & mask) {
                continue;
            }
        }
        *cursor = a->id;
        {
            u8 *base = (u8 *)a + 0x50;
            if (Inactive(a)) {
                continue;
            }
            if (center->x - radius->x > *(s16 *)(base + 12) ||
                ((center->x + radius->x) < (*((s16 *)(base + 12))))) {
                DEBUG_NOP();
                continue;
            }
            if (center->y - radius->y > *(s16 *)(base + 14) ||
                ((center->y + radius->y) < (*((s16 *)(base + 14))))) {
                continue;
            }
            if (center->z - radius->z > *(s16 *)(base + 16) ||
                ((center->z + radius->z) < (*((s16 *)(base + 16))))) {
                continue;
            }
            return a;
        }
    } while (++i <= 19);
    return 0;
}
