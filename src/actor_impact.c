#include "actor_impact.h"
#include "engine.h"
#include "ram.h"

s32 Math_Atan2(s32, s32);

static inline u32 TestColliderFlags(struct ContactCollider *source, u32 mask)
{
    return source->flags & mask;
}

void CollisionReaction_RecordImpact(struct ContactCollider *source, struct ContactCollider *target,
                                    struct ActorImpact *actor)
{
    s32 delta = actor->direction - actor->body.direction;
    s32 distance;
    u32 reward;
    if (delta > 128)
        delta -= 256;
    else if (delta < -128)
        delta += 256;
    if (delta < 0)
        delta = -delta;
    distance = delta;
    if (TestColliderFlags(source, 0x100))
        actor->hitDirection = source->direction;
    else
        actor->hitDirection = Math_Atan2(target->x - source->x, target->z - source->z);
    if (distance <= 32)
        actor->frontalImpact = 1;
    reward = source->statRewardFlags;
    actor->rewardFlags = reward;
}

void Collision_CalculateDamageResponse(struct ContactCollider *, struct ContactCollider *);

void PlayerStats_AddContactHundredths(struct ContactCollider *, struct ContactCollider *, s32);

static inline void SetRewardMask(struct ActorImpact *boss, u32 value)
{
    boss->rewardMask = value;
}

static inline void SetHitAttributes(struct ActorImpact *boss, u32 value)
{
    boss->contactAttributes = value;
}

void ActorImpact_CopyContact(struct ContactCollider *attack, struct ContactCollider *defense,
                             struct ActorImpact *actor)
{
    struct ActorImpact *boss = actor;
    u32 rewardMask;
    u32 attributes;
    u16 angleFlag;
    Collision_CalculateDamageResponse(attack, defense);
    actor->damage = defense->cost;
    angleFlag = 0x100;
    if (attack->flags & angleFlag)
        actor->hitDirection = attack->direction;
    else
        actor->hitDirection = Math_Atan2(defense->x - attack->x, defense->z - attack->z);
    actor->hitParameter = attack->responseParameter;
    rewardMask = attack->statRewardFlags;
    SetRewardMask(boss, rewardMask);
    attributes = attack->reactionFlags;
    SetHitAttributes(boss, attributes);
    PlayerStats_AddContactHundredths(attack, defense, boss->damageBonus);
    {
        u32 one = 1;
        if (attack->reactionFlags & one)
            actor->hitByLight = one;
    }
    attributes = attack->reactionFlags;
    actor->hitAttributes = attributes;
    actor->response = attack->responseValue;
    PlayerStats_AddContactHundredths(attack, defense, 12);
}

/* Empty function; semantic role is not established. */

void ActorImpact_NoOp(void)
{
}

void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);

void ActorImpact_InitAttackCollider(struct ActorImpact *actor)
{
    struct ContactPackedVector size;
    union ActorImpactVector offset;
    struct ContactCollider *collider = &actor->attackCollider;
    size.x = 64;
    size.y = 128;
    size.z = 64;
    offset.words[0] = 0x800000;
    offset.words[1] &= 0xffff0000;
    Collider_Init(collider, 0, 0x2001, 0, 16, &size, &offset);
    Collider_SetPosition(collider, &actor->colliderPosition, 0);
    Collider_SetCallback(collider, ActorImpact_NoOp, actor);
    Collider_SetAttackProperties(collider, 0, 0, 0, 0, 0);
}

void Collider_SetBodyProperties(void *, s32, s32, s32);

void ActorImpact_InitBodyCollider(struct ActorImpact *actor, s32 resource)
{
    struct ContactPackedVector size;
    union ActorImpactVector offset;
    struct ContactCollider *collider = &actor->bodyCollider;
    size.x = 128;
    size.y = 256;
    size.z = 128;
    offset.words[0] = 0x800000;
    offset.words[1] &= 0xffff0000;
    Collider_Init(collider, 0, 0x5101, 0, 16, &size, &offset);
    Collider_SetPosition(collider, &actor->colliderPosition, 0);
    Collider_SetCallback(collider, ActorImpact_CopyContact, actor);
    Collider_SetBodyProperties(collider, resource, 0x802, 1);
    Collider_Refresh(collider);
}

void ActorImpact_InitCollider(struct ActorImpact *actor)
{
    struct ContactPackedVector size;
    union ActorImpactVector offset;
    struct ContactCollider *collider = &actor->proximityCollider;
    size.x = 256;
    size.y = 128;
    size.z = 256;
    offset.words[0] = 0;
    offset.words[1] &= 0xffff0000;
    Collider_Init(collider, 0, 0x4002, 0, 16, &size, &offset);
    Collider_SetPosition(collider, (u8 *)&actor->colliderPosition, 0);
    Collider_SetCallback(collider, CollisionReaction_RecordImpact, actor);
    Collider_SetBodyProperties(collider, 0, 0x802, 1);
    Collider_Refresh(collider);
}

/* The original ABI leaves r0 incidental; callers must not consume a defined result. */

u32 ActorExtension_ResetState(struct ActorImpact *actor)
{
    actor->damage = 0;
    actor->hitByLight = 0;
    actor->hitAttributes = 0;
}

s32 ActorResource_IsBelowSelectedLimit(struct ActorImpact *actor, s32 value)
{
    if (actor->resourceSelection == 0) {
        if (value < actor->resourceLimits[0])
            return 1;
    } else if (actor->resourceSelection == 1) {
        if (value < actor->resourceLimits[1])
            return 1;
    }
    return 0;
}

s32 ActorResource_ClampDecrease(struct ActorImpact *actor, s32 decrease)
{
    s32 result;
    s32 current = actor->currentResource;
    s32 next = current - decrease;
    if (actor->resourceSelection == 0) {
        s32 limit = actor->resourceLimits[0];
        if (next < limit) {
            result = current + 1;
            result -= limit;
            return result;
        }
    } else if (actor->resourceSelection == 1) {
        s32 limit = actor->resourceLimits[1];
        if (next < limit) {
            result = current + 1;
            result -= limit;
            return result;
        }
    }
    return decrease;
}

extern const s16 gUnk_086149C4[];

u8 sub_0821D4F0(void *, void *, u32, u32);

static inline s32 ActorImpact_TruncateFixed(s32 components)
{
    if (components >= 0)
        return components >> 12;
    return -((-components) >> 12);
}

u8 ActorImpact_TestForwardPoint(struct ActorImpact *actor)
{
    struct ContactPosition point;
    u8 *anglePtr = &actor->body.direction;
    s16 *components = (s16 *)&point;
    u32 angle = *anglePtr;
    s32 speed = 256;
    {
        s32 x = ActorImpact_TruncateFixed(gUnk_086149C4[(angle + 64) & 255] * 256);
        s32 zero = 0;
        components[0] = x;
        components[1] = zero;
    }
    {
        s32 sample = gUnk_086149C4[angle];
        components[2] = ActorImpact_TruncateFixed(sample * speed);
    }
    {
        struct ContactPosition *destination = &point;
        struct ContactPosition *origin = &actor->body.position;
        u8 result;
        point.x = (u16)point.x + (u16)origin->x;
        destination->y = (u16)destination->y + (u16)origin->y;
        {
            u32 z = (u16)origin->z;
            u32 delta = (u16)destination->z;
            destination->z = z + delta;
        }
        if (sub_0821D4F0(origin, &point, 2, 256) != 0)
            return 1;
        return 0;
    }
}

s32 AreaEffect_IsPositionCovered(void *);

s32 SceneActors_GetEffectiveTimeStep(struct ActorImpact *actor)
{
    s32 result;
    if (gPlayerEffectPendingCount != 0) {
        result = 4;
    } else if (AreaEffect_IsPositionCovered(&actor->body.position) == 0) {
        result = 0;
    } else {
        result = gGameState->unk876;
    }
    return result;
}

void ActorImpact_FindNearbyPoint(struct ActorImpact *actor, struct ContactPosition *out,
                                 u32 start)
{
    struct ContactPosition point;
    struct SceneActorBody *body = &actor->body;
    s32 attempt = 0;
    u32 mask = 255;
    u32 speed = 256;
    s16 *components = (s16 *)&point;
    struct ContactPosition *center = &actor->body.position;
    for (; attempt <= 7; attempt++) {
        u32 angle = (start + (attempt << 5)) & mask;
        {
            s32 x = ActorImpact_TruncateFixed(gUnk_086149C4[(angle + 64) & mask] * 256);
            s32 zero = 0;
            components[0] = x;
            components[1] = zero;
        }
        {
            s32 sample = gUnk_086149C4[angle & mask];
            s32 value = sample * speed;
            s32 z = ActorImpact_TruncateFixed(value);
            components[2] = z;
            {
                u32 x = (u16)body->position.x;
                u32 dx = (u16)components[0];
                components[0] = x + dx;
            }
            components[1] = (u16)components[1] + (u16)center->y;
            components[2] += center->z;
        }
        if (sub_0821D4F0(center, &point, 3, 256)) {
            *out = point;
            return;
        }
    }
}
