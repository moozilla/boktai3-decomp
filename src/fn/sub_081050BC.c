#include "global.h"
#include "scene_object.h"
#include "callback_pool.h"
#include "engine.h"

extern const u16 gUnk_0203B400[];
void sub_08014634(u32, u32, struct SceneExtent *, struct SceneExtent *, struct SceneExtent *, u32,
                  u32);

static inline void ClearVector(void *dest, u32 *zero)
{
    *zero = 0;
    CpuSet(zero, dest, 0x05000002);
}

static inline s32 NextRandom(void)
{
    const u16 *table = gUnk_0203B400;
    gRandomIndex = (gRandomIndex + 1) & 0x3ff;
    return table[gRandomIndex];
}

// confidence=low: spawns a small burst of hit particles at the entry's position.
void CallbackPool_SpawnHitParticles(struct CallbackPoolEntry *entry)
{
    u32 stack[3];
    struct SceneExtent spread;
    struct SceneExtent *velocity = (struct SceneExtent *)(stack + 1);
    ClearVector(velocity, stack);
    ((u16 *)velocity)[1] = 16;
    spread.x = (NextRandom() & 3U) + 15U;
    spread.y = (NextRandom() & 7U) + 15U;
    spread.z = (NextRandom() & 3U) + 15U;
    sub_08014634((NextRandom() & 3) + 2, 17, (struct SceneExtent *)((u8 *)entry + 0x1c), velocity,
                 &spread, 12, 15);
}
