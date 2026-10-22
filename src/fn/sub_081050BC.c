// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_object.h"
#include "callback_pool.h"

#ifdef NONMATCHING
// fuzzy: 90.7%
// Open: reload copies the spread mask 3 (kept in sb) through r1 where the ROM uses r7,
// which also swaps the reload register for the 0xFFFF0000 mask. An empty asm keeps the
// count's mask 3 out of CSE so the two masks get the ROM's separate pseudos.
#define gShakeRandomTable gUnk_0203B400
#define gShakeRandomIndex gUnk_03005308
extern const u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
void sub_08014634(u32, u32, struct SceneExtent *, struct SceneExtent *, struct SceneExtent *, u32,
                  u32);
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
    {
        u32 count = NextRandom();
        u32 three;
        asm("" : "=r"(three) : "0"(3));
        count &= three;
        count += 2;
        entry = (struct CallbackPoolEntry *)((u8 *)entry + 0x1c);
        sub_08014634(count, 17, (struct SceneExtent *)entry, velocity, &spread, 12, 15);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", CallbackPool_SpawnHitParticles);
#endif
