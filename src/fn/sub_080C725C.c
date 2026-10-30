#include "global.h"
#include "scene_object.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"

struct SceneActorPlacementView;
void SceneActorPlacement_PlayVariantEventSound(struct SceneActorPlacementView *, u32);
u32 ActorMode_SelectOddStateVariant(struct SceneActorRegistryResetView *);
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

static inline u32 GolemSlide_ReadSampleAddress(u32 address, u32 origin, const u16 *table)
{
    u32 offset = address - origin;
    return *(const u16 *)((const u8 *)table + offset);
}
static inline u32 GolemSlide_NextBankSample(u32 origin, const u16 *table)
{
    u32 address;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    address = origin + gRandomIndex * sizeof(*table);
    return GolemSlide_ReadSampleAddress(address, origin, table);
}

static inline s32 GolemSlide_Quarter(s16 value)
{
    if (value >= 0)
        return value >> 2;
    return -((-value) >> 2);
}

static inline void GolemSlide_SetMotionFlags(struct SceneActorRegistryResetView *actor, u16 mask)
{
    actor->flags2D4 = mask | actor->flags2D4;
}

static inline void GolemSlide_ClearColliderFlags(struct ContactCollider *collider, u16 bits)
{
    u32 mask = ~bits;
    collider->flags = mask & collider->flags;
}

static inline void GolemSlide_SetDelay(struct SceneActorRegistryResetView *actor, u16 delay)
{
    actor->fieldAA = delay;
}

static inline void GolemSlide_SetStatusFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    actor->flags2D0 |= mask;
}

void Golem_UpdateSlideImpact(struct SceneActorRegistryResetView *actor, u32 frame)
{
    struct GolemSlideContext *context = actor->context;
    struct SceneActorChaseGroup *group = &actor->group;
    struct SceneActorBody *body = &actor->body;
    if (SceneActorReaction_TakePrepared(actor))
        SceneActorPlacement_PlayVariantEventSound((void *)actor, 4);
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 15);
    SceneActorChase_SetReactionFlags(actor, 0x20);
    GolemSlide_SetMotionFlags(actor, 0x100);
    SceneActorChase_SetActorFlags(actor, 0x2000);
    actor->body.motion.x += (context->speed * gUnk_086149C4[(body->direction + 64) & 255]) >> 16;
    actor->body.motion.z += (context->speed * gUnk_086149C4[body->direction]) >> 16;
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
    if (actor->unknown9[0x40 - 9] & 1) {
        struct SceneExtent velocity, spread;
        u32 count;
        u32 randomOrigin;
        context->impactDelay = 30;
        context->colliderDelay = 20;
        velocity.x = GolemSlide_Quarter(body->motion.z);
        /* The recovered origin cancels when a later sample address becomes a table offset. */
        randomOrigin = 0x02000714u + 0x0203B000u;
        velocity.y = GolemSlide_Random((const u16 *)(randomOrigin - 0x02000714u + 0x400)) & 7;
        velocity.z = GolemSlide_Quarter(body->motion.x);
        spread.x = (GolemSlide_NextBankSample(randomOrigin, gRandomTable) & 3) + 15;
        spread.y = (GolemSlide_NextBankSample(randomOrigin, gRandomTable) & 7) + 15;
        spread.z = (GolemSlide_NextBankSample(randomOrigin, gRandomTable) & 3) + 15;
        count = (GolemSlide_NextBankSample(randomOrigin, gRandomTable) & 3) + 4;
        sub_08014634(count, ActorMode_SelectOddStateVariant(actor), (void *)&body->position,
                     &velocity, &spread, 45, 31);
        SceneActorPlacement_PlayVariantEventSound((void *)actor, 5);
        if (context->recovery) {
            actor->field2B0 = 1;
        } else {
            GolemSlide_SetDelay(actor, 180);
            GolemSlide_SetStatusFlags(actor, 1);
            SceneActorChase_SetState(actor, 3, 0, 0);
            SceneActorChase_SetControl(actor, group->contactControl, 7);
            GolemSlide_ClearColliderFlags(&actor->colliderA, 0x100);
        }
    }
}
