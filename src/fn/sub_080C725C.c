#include "global.h"
#include "monster_particle_random.h"
#include "scene_object.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"

struct RoomActor;
void SceneActorPlacement_PlayVariantEventSound(struct RoomActor *, u32);
u32 ActorMode_SelectOddStateVariant(struct ChaseActor *);
void sub_08014634(u32, u32, struct SceneExtent *, struct SceneExtent *, struct SceneExtent *, u32,
                  u32);

struct GolemSlideContext {
    u8 unknown0[0x634];
    u16 recovery;
    u8 unknown636[0x4A];
    u16 state;
    s16 speed;
    u8 unknown684[0x28];
    u16 impactDelay, colliderDelay;
};

static inline u32 GolemSlide_Random(const u16 *table)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return table[gRandomIndex];
}

static inline s32 GolemSlide_Quarter(s16 value)
{
    if (value >= 0)
        return value >> 2;
    return -((-value) >> 2);
}

static inline void GolemSlide_SetMotionFlags(struct ChaseActor *actor, u16 mask)
{
    actor->base.motionFlags = mask | actor->base.motionFlags;
}

static inline void GolemSlide_ClearColliderFlags(struct ContactCollider *collider, u16 bits)
{
    u32 mask = ~bits;
    collider->flags = mask & collider->flags;
}

static inline void GolemSlide_SetDelay(struct ChaseActor *actor, u16 delay)
{
    actor->base.attachmentTimer = delay;
}

static inline void GolemSlide_SetStatusFlags(struct ChaseActor *actor, u32 mask)
{
    actor->base.statusFlags |= mask;
}

void Golem_UpdateSlideImpact(struct ChaseActor *actor, u32 frame)
{
    struct GolemSlideContext *context = actor->base.context;
    struct SceneActorChaseGroup *group = &actor->group;
    struct SceneActorBody *body = &actor->base.body;
    if (SceneActorReaction_TakePrepared(actor))
        SceneActorPlacement_PlayVariantEventSound((void *)actor, 4);
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 15);
    SceneActorChase_SetReactionFlags(actor, 0x20);
    GolemSlide_SetMotionFlags(actor, 0x100);
    SceneActorChase_SetActorFlags(actor, 0x2000);
    actor->base.body.motion.x +=
        (context->speed * gUnk_086149C4[(body->direction + 64) & 255]) >> 16;
    actor->base.body.motion.z += (context->speed * gUnk_086149C4[body->direction]) >> 16;
    if ((frame & 3) == 0) {
        struct SceneExtent velocity, spread;
        velocity.x = -body->motion.x;
        velocity.y = 0;
        velocity.z = -body->motion.z;
        spread.x = 32;
        spread.y = 0;
        spread.z = 32;
        SceneParticleBurst_CreateWithPause((void *)&body->position, (s32)&velocity, &spread, 1, 3,
                                           4);
    }
    if (actor->base.unknown9[0x40 - 9] & 1) {
        struct SceneExtent velocity, spread;
        u32 count;
        u32 randomOrigin;
        context->impactDelay = 30;
        context->colliderDelay = 20;
        velocity.x = GolemSlide_Quarter(body->motion.z);
        /* The recovered origin cancels when a later sample address becomes a table offset. */
        PARTICLE_RANDOM_CAPTURE_ORIGIN(randomOrigin);
        velocity.y = GolemSlide_Random((const u16 *)RANDOM_TABLE_ADDR) & 7;
        velocity.z = GolemSlide_Quarter(body->motion.x);
        spread.x = (ParticleRandom_ReadRelativeSample(randomOrigin, gRandomTable) & 3) + 15;
        spread.y = (ParticleRandom_ReadRelativeSample(randomOrigin, gRandomTable) & 7) + 15;
        spread.z = (ParticleRandom_ReadRelativeSample(randomOrigin, gRandomTable) & 3) + 15;
        count = (ParticleRandom_ReadRelativeSample(randomOrigin, gRandomTable) & 3) + 4;
        sub_08014634(count, ActorMode_SelectOddStateVariant(actor), (void *)&body->position,
                     &velocity, &spread, 45, 31);
        SceneActorPlacement_PlayVariantEventSound((void *)actor, 5);
        if (context->recovery) {
            actor->base.stepDone = 1;
        } else {
            GolemSlide_SetDelay(actor, 180);
            GolemSlide_SetStatusFlags(actor, 1);
            SceneActorChase_SetState(actor, 3, 0, 0);
            SceneActorChase_SetControl(actor, group->contactControl, 7);
            GolemSlide_ClearColliderFlags(&actor->base.colliderA, 0x100);
        }
    }
}
