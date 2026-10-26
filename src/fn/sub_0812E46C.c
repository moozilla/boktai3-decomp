#include "global.h"
#include "scene_object.h"
#include "scene_actor_reset.h"

extern const s16 gUnk_086149C4[];
u32 ActorMode_SelectOddStateVariant(struct SceneActorRegistryResetView *);
void sub_08014634(u32, u32, struct SceneExtent *, struct SceneExtent *, struct SceneExtent *, u32,
                  u32);

static inline s32 ActorParticles_NextRandom(void)
{
    const u16 *table = gUnk_0203B400;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return table[gRandomIndex];
}

void ActorParticles_SpawnDirectionalBurst(struct SceneActorRegistryResetView *actor, s32 amount)
{
    struct SceneExtent velocity, spread;
    u32 count;
    u32 angle;
    if (amount > 0) {
        angle = (actor->field2B6 + 128) & 255;
        velocity.x = gUnk_086149C4[(angle + 64) & 255] >> 10;
        velocity.y = ActorParticles_NextRandom() & 7;
        velocity.z = gUnk_086149C4[angle & 255] >> 10;
        spread.x = (ActorParticles_NextRandom() & 7) + 3;
        spread.y = (ActorParticles_NextRandom() & 7) + 3;
        spread.z = (ActorParticles_NextRandom() & 7) + 3;
        count = (ActorParticles_NextRandom() & 1) + 2;
        sub_08014634(count, ActorMode_SelectOddStateVariant(actor),
                     (struct SceneExtent *)&actor->position, &velocity, &spread, 45, 31);
    }
}
