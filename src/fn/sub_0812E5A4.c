#include "global.h"
#include "scene_object.h"
#include "scene_actor_reset.h"
#ifdef NONMATCHING
// fuzzy: 84.8%
// foundry draft (worker best 83.4%); what differs is mostly register allocation and instruction order
#define gRandomTableIndex         gUnk_03005308
#define gRandomTable              gUnk_0203B400
#define ActorParticles_SpawnBurst sub_0812E5A4
u32 ActorMode_SelectOddStateVariant(struct SceneActorRegistryResetView *);
void sub_08014634(u32, u32, struct SceneExtent *, struct SceneExtent *, struct SceneExtent *, u32,
                  u32);
static inline u16 ActorParticles_NextRandom(void)
{
    const u16 *table = gUnk_0203B400;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return table[gUnk_03005308];
}
static inline s32 ActorParticles_Half(s16 n)
{
    if (n >= 0)
        return n >> 1;
    return (s16) - ((-n) >> 1);
}
void sub_0812E5A4(struct SceneActorRegistryResetView *actor)
{
    struct SceneExtent velocity, spread;
    u32 count;
    velocity.x = ActorParticles_Half(actor->motion.x);
    velocity.y = ActorParticles_NextRandom() & 7;
    velocity.z = ActorParticles_Half(actor->motion.z);
    spread.x = (ActorParticles_NextRandom() & 3) + 15;
    spread.y = (ActorParticles_NextRandom() & 7) + 15;
    spread.z = (ActorParticles_NextRandom() & 3) + 15;
    count = (ActorParticles_NextRandom() & 3) + 4;
    sub_08014634(count, ActorMode_SelectOddStateVariant(actor),
                 (struct SceneExtent *)&actor->position, &velocity, &spread, 45, 31);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0812E5A4);
#endif
