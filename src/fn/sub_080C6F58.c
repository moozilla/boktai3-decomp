#include "golem_charge.h"
#include "monster_particle_random.h"
#include "scene_actor_reaction.h"
#include "scene_object.h"

static inline s32 Scale8(s16 n)
{
    return n >= 0 ? n >> 8 : -((-n) >> 8);
}
/* Charge windup: reset the pending-hit count, emit dust, and ramp the stored speed. */
// confidence=low: charge controls 080CDF14 and 080CEDF0 install this preparatory state.
void Golem_UpdateChargeWindup(struct SceneActorRegistryResetView *actor, u32 frame)
{
    struct GolemChargeContext *context = actor->context;
    struct SceneActorBody *body = &actor->body;
    u8 directionVariant, mirror;
    s16 *speed;
    if (SceneActorReaction_TakePrepared(actor)) {
        actor->field2B3 = 0;
        context->pendingHits = 0;
    }
    Golem_GetDirectionVariant(actor, &directionVariant, &mirror);
    Golem_SelectAnimation(actor, 8, directionVariant, Golem_MirrorFlags(mirror));
    {
        u32 mask = 0x20;
        mask |= actor->flags2D6;
        actor->flags2D6 = mask;
    }
    Golem_SetMotionFlags(actor, 0x100);
    if ((frame & 3) == 0) {
        struct SceneExtent velocity, size;
        u32 origin, savedOrigin, direction, count;
        PARTICLE_RANDOM_CAPTURE_ORIGIN(origin);
        direction = ParticleRandom_ReadDirectSample((const u16 *)RANDOM_TABLE_ADDR) & 0xFF;
        velocity.x = Scale8(gUnk_086149C4[(direction + 0x40) & 0xFF]);
        velocity.y = 0;
        PARTICLE_RANDOM_SAVE_ORIGIN(savedOrigin, origin);
        velocity.z = Scale8(gUnk_086149C4[(direction + 0x40) & 0xFF]);
        size.x = 48;
        size.y = 0;
        size.z = 48;
        count =
            (ParticleRandom_ReadRelativeSample(savedOrigin, (const u16 *)RANDOM_TABLE_ADDR) & 1) +
            2;
        SceneParticleBurst_CreateWithPause((void *)&body->position, (s32)&velocity, &size, 2, count,
                                           4);
    }
    speed = &context->charge.speed;
    if (*speed <= 383)
        *speed += 16;
    actor->field2C2 = 0x40 >> (*speed >> 7);
    Golem_ApplyDuration(actor);
}
