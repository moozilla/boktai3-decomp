#include "actor_pool.h"

void Actor_RequestRemoval(struct PooledActor *actor)
{
    u32 m = 1;
    actor->flags |= m;
}

void Actor_InitEntity(struct PooledActor *actor, u32 id, u8 direction,
                      const struct ContactPosition *position)
{
    SceneBody_Init(&actor->body, id, position, direction, 7, actor);
}

void Actor_SetFacingAngle(struct PooledActor *actor, int angle)
{
    actor->body.direction = angle;
    actor->facing8 = SceneFacing_EightWay(angle);
    actor->facing4 = SceneFacing_FourWay(angle);
}

void Actor_SetLifecycleCallbacks(struct PooledActor *actor, void (*update)(void *),
                                 void (*destroy)(void *))
{
    actor->update = update;
    actor->destroy = destroy;
}

void Actor_SetHealthLimits(struct PooledActor *actor, u32 maximum, u32 health)
{
    actor->maxHealth = maximum;
    Actor_SetHealth(actor, health);
}

void Actor_SetHealth(struct PooledActor *actor, int health)
{
    actor->health = health;
    if (health < 0)
        actor->health = 0;
    else if (health > actor->maxHealth)
        actor->health = actor->maxHealth;
}

void Actor_SubtractHealth(struct PooledActor *actor, s32 damage)
{
    actor->health -= damage;
    if (actor->health < 0)
        actor->health = 0;
}

void Actor_CapturePendingHit(struct PooledActor *actor, struct ContactCollider *self,
                             struct ContactCollider *other)
{
    if (!actor->pendingDamage) {
        actor->pendingDamage = other->cost;
        actor->damageSource = self->statRewardFlags;
        actor->attackFlags = self->reactionFlags;
        {
            u32 value = self->responseValue;
            actor->reaction = value;
        }
    }
}

void Actor_ClearPendingHit(struct PooledActor *actor)
{
    actor->pendingDamage = 0;
    actor->damageSource = 0;
    actor->attackFlags = 0;
}

void Actor_InitHitSensor(struct PooledActor *actor, void *a, void *b, void *fn)
{
    struct ContactCollider *q = &actor->hitSensor;
    u32 flags = 0x2001;
    Collider_Init(q, 0, flags, 0, 16, a, b);
    Collider_SetPosition(q, &actor->body.position, 0);
    Collider_SetCallback(q, fn, actor);
    Collider_SetAttackProperties(q, actor->attackProperty0, actor->attackProperty1,
                                 actor->attackProperty3, actor->attackProperty4,
                                 actor->attackProperty2);
    {
        u32 mask = 4;
        actor->flags |= mask;
    }
}

void Actor_InitBodyCollider(struct PooledActor *actor, void *a, void *b, void *fn)
{
    struct ContactCollider *q = &actor->bodyCollider;
    u32 flags = 0x4001;
    Collider_Init(q, 0, flags, 0, 16, a, b);
    Collider_SetPosition(q, &actor->body.position, 0);
    Collider_SetCallback(q, fn, actor);
    Collider_SetBodyProperties(q, actor->bodyProperty, actor->bodyProperty1, actor->bodyProperty2);
    Collider_Refresh(q);
    {
        u32 mask = 8;
        actor->flags |= mask;
    }
}

struct SceneActorBody *Actor_GetPartyTarget(u32 slot)
{
    struct ActorPoolManager *manager = ActorPool_GetManager();
    if (!manager)
        return 0;
    return manager->targets[slot];
}

int Actor_FindTargetInFacingArc(struct PooledActor *actor)
{
    int i;
    if (!ActorPool_GetManager())
        return -1;
    for (i = 0; i < (s8)actor->targetCount; i++) {
        struct SceneActorBody *t = Actor_GetPartyTarget(i);
        if (t) {
            struct ContactPosition *src = &actor->body.position;
            struct ContactPosition *dst = &t->position;
            int angle = Math_Atan2(dst->x - src->x, dst->z - src->z);
            int facing = actor->body.direction;
            int signedAngle = (s8)angle;
            u8 diff;
            int magnitude;
            diff = facing - signedAngle;
            magnitude = (s8)diff;
            if (magnitude < 0)
                magnitude = -magnitude;
            if (magnitude <= actor->targetArc) {
                int dx = dst->x - src->x;
                int dz = dst->z - src->z;
                u32 distance = dx * dx + dz * dz;
                if (distance <= actor->targetRangeSquared) {
                    actor->targetAngleDelta = diff;
                    actor->targetDistanceSquared = distance;
                    return i;
                }
            }
        }
    }
    return -1;
}

void Actor_RefreshTarget(struct PooledActor *actor)
{
    struct SceneActorBody *t;
    if (actor->targetIndex < 0)
        return;
    t = Actor_GetPartyTarget(actor->targetIndex);
    if (t) {
        struct ContactPosition *src = &actor->body.position;
        struct ContactPosition *dst = &t->position;
        int angle = Math_Atan2(dst->x - src->x, dst->z - src->z);
        int facing = actor->body.direction;
        int signedAngle = (s8)angle;
        u8 diff;
        int dx, dz, magnitude;
        u32 distance;
        diff = facing - signedAngle;
        dx = dst->x - src->x;
        dz = dst->z - src->z;
        distance = dx * dx + dz * dz;
        magnitude = (s8)diff;
        if (magnitude < 0)
            magnitude = -magnitude;
        if (magnitude <= actor->targetArc && distance <= actor->targetRangeSquared) {
            actor->targetAngleDelta = diff;
            actor->targetDistanceSquared = distance;
        } else
            actor->targetIndex = -1;
    } else
        actor->targetIndex = -1;
}

void Actor_UpdateSelectedTarget(struct PooledActor *actor)
{
    struct ActorPoolManager *m = ActorPool_GetManager();
    int selected = 0;
    if (m->updateSlot == actor->updateSlot)
        selected = 1;
    if (selected) {
        if (actor->targetIndex >= 0)
            Actor_RefreshTarget(actor);
        else
            actor->targetIndex = Actor_FindTargetInFacingArc(actor);
    }
}

void Actor_StepFacingAngle(struct PooledActor *actor)
{
    if (actor->targetIndex >= 0) {
        s8 *deltaPtr = (s8 *)(&actor->targetAngleDelta);
        int delta;
        int angle;
        if ((*deltaPtr) > 3) {
            angle = actor->body.direction + 4;
        } else if ((*deltaPtr) <= (-4)) {
            angle = actor->body.direction - 4;
        } else {
            angle = (*deltaPtr) + actor->body.direction;
        }
        Actor_SetFacingAngle(actor, angle);
    }
}

void Actor_SetActionParameters(struct PooledActor *actor, u32 value, s32 count, u32 *src)
{
    actor->action = value;
    if (count > 0) {
        u32 *dst = actor->actionParameters;
        do {
            *dst++ = *src++;
        } while (--count != 0);
    }
}
