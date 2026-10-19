
#ifdef NONMATCHING
// fuzzy: 83.8%
// foundry draft (worker best 82.7%); what differs is mostly register allocation and instruction order
#define CallbackPool_SpawnHitParticles sub_081050BC
#define gShakeRandomTable gUnk_0203B400
#define gShakeRandomIndex gUnk_03005308
#include "global.h"
#include "scene_object.h"
#include "callback_pool.h"
extern const u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
void sub_08014634(u32, u32, struct SceneExtent *, struct SceneExtent *, struct SceneExtent *, u32, u32);
static inline void ClearVector(void *dest, u32 *zero)
{
    *zero = 0;
    CpuSet(zero, dest, 0x05000002);
}
static inline u16 NextRandom(void)
{
    const u16 *table = gUnk_0203B400;
    gUnk_03005308 = (gUnk_03005308 + 1) & 0x3ff;
    return table[gUnk_03005308];
}
void sub_081050BC(struct CallbackPoolEntry *entry)
{
    u32 stack[3];
    struct SceneExtent spread;
    struct SceneExtent *velocity = (struct SceneExtent *)(stack + 1);
    ClearVector(velocity, stack);
    ((u16 *)velocity)[1] = 16;
    spread.x = (NextRandom() & 3U) + 15U;
    spread.y = (NextRandom() & 7U) + 15U;
    spread.z = (NextRandom() & 3U) + 15U;
    {
        u32 count = (NextRandom() & 3) + 2;
        entry = (struct CallbackPoolEntry *)((u8 *)entry + 0x1c);
        sub_08014634(count, 17, (struct SceneExtent *)entry, velocity, &spread, 12, 15);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081050BC);
#endif
