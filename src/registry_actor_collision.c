#include "registry_actor_collision.h"

static inline s32 RegistryActor_ScaleVelocity(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

static inline u8 RegistryActor_HasPendingDamage(struct RegistryActor *actor)
{
    u32 mask = 1;
    if (actor->stats.damageFlags & mask)
        return TRUE;
    return FALSE;
}

static inline bool32 RegistryActor_IsDamageKind(s32 kind, s32 want)
{
    if (kind == want)
        return TRUE;
    return FALSE;
}

static inline void RegistryActor_PolarOffset(struct ContactPosition *v, s32 angle, s32 speed)
{
    v->x = RegistryActor_ScaleVelocity(gUnk_086149C4[(angle + 64) & 255] * speed);
    v->y = 0;
    v->z = RegistryActor_ScaleVelocity(gUnk_086149C4[angle & 255] * speed);
}

void Actor_ApplyKnockback(struct RegistryActor *actor, struct RegistryCollisionMessage *msg)
{
    struct {
        struct RegistryIntegerVector target;
        struct RegistryIntegerVector self;
        struct RegistryIntegerVector diff;
    } positions;
    struct ContactPosition vel;
    s32 angle;
    s32 t;

    if (RegistryActor_HasPendingDamage(actor))
        return;
    if (msg->type == 7) {
        actor->stats.damageTimer = 0;
        return;
    }
    if (RegistryActor_IsDamageKind(msg->kind, 4) || RegistryActor_IsDamageKind(msg->kind, 3)) {
        actor->stats.damageTimer = 0;
        return;
    }
    actor->stats.pendingDamage = msg->value;
    actor->stats.damageType = msg->type;
    {
        u8 one = 1;
        actor->stats.damageFlags |= one;
    }
    positions.self.x = actor->stats.contactPosition.x + actor->stats.contactOffset.x;
    positions.self.y = actor->stats.contactPosition.y + actor->stats.contactOffset.y;
    positions.self.z = actor->stats.contactPosition.z + actor->stats.contactOffset.z;
    positions.target.x = msg->position.x + msg->offset.x;
    positions.target.y = msg->position.y + msg->offset.y;
    positions.target.z = msg->position.z + msg->offset.z;
    positions.diff.x = positions.self.x - positions.target.x;
    positions.diff.y = positions.self.y - positions.target.y;
    positions.diff.z = positions.self.z - positions.target.z;
    angle = Math_Atan2(positions.diff.x, positions.diff.z);
    angle = (angle >> 5) << 5;
    RegistryActor_PolarOffset(&vel, angle, msg->speed);
    t = actor->stats.secondaryConfig[0] >> 1;
    actor->stats.knockback.forward = vel.x << 10;
    actor->stats.knockback.lateral = vel.z << 10;
    actor->stats.knockbackTimer = 60 - t;
    if (actor->stats.knockbackTimer < 10)
        actor->stats.knockbackTimer = 10;
    if ((u32)(angle - 10) > 0xEC) {
        actor->stats.motion.forward += 0x3000;
    } else {
        actor->stats.motion.forward -= 0x100;
        actor->stats.boost = 0;
    }
}



void ActorMotion_SetKnockback(struct RegistryActor *actor, struct RegistryCollisionMessage *other)
{
    struct RegistryIntegerVector them, self, delta;
    struct ContactPosition dir;
    struct RegistryCollisionMessage *body = (void *)actor->stats.contactHeader;
    int angle;
    self.x = body->position.x + body->offset.x;
    self.y = body->position.y + body->offset.y;
    self.z = body->position.z + body->offset.z;
    them.x = other->position.x + other->offset.x;
    them.y = other->position.y + other->offset.y;
    them.z = other->position.z + other->offset.z;
    delta.x = self.x - them.x;
    delta.y = self.x - them.x;
    delta.z = self.x - them.x;
    angle = Math_Atan2(delta.x, delta.z);
    RegistryActor_PolarOffset(&dir, angle, 10);
    actor->stats.knockback.forward = dir.x << 12;
    actor->stats.knockback.lateral = dir.z << 12;
    actor->stats.knockbackTimer = 30;
}

void ActorCollision_InitBounds(struct RegistryActor *actor)
{
    union RegistryCollisionVector position;
    union RegistryCollisionVector size, offset;
    s32 y;
    position.unsignedPacked.x = (u16)actor->model.view.parts[0].x;
    position.unsignedPacked.y = (u16)actor->model.view.parts[0].y - 96;
    position.unsignedPacked.z = (u16)actor->model.view.parts[0].y - 96;
    size.packed.x = 24;
    size.packed.y = 32;
    size.packed.z = 2;
    offset.packed.x = 0;
    offset.packed.y = -(s16)size.packed.y;
    offset.packed.z = -size.packed.z;
    SpatialIndex_InitEntry(actor->stats.contactHeader, &position, &size, &offset, 20, 5, 1, 0, 12,
                           Actor_ApplyKnockback, actor);
    position.unsignedPacked.x = (u16)actor->model.view.parts[0].x;
    y = (u16)actor->model.view.parts[0].y - 96;
    position.components[1] = y;
    position.components[2] = y;
    size.components[0] = 24;
    size.components[1] = 32;
    size.components[2] = 2;
    offset.components[0] = 0;
    offset.components[1] = -size.unsignedComponents[1];
    offset.components[2] = -size.unsignedComponents[2];
    SpatialIndex_InitEntry(actor->stats.collisionBody, &position, &size, &offset, 12, 5, 1, 0, 12,
                           ActorMotion_SetKnockback, actor);
    SpatialIndex_InsertEntry(actor->stats.contactHeader);
}

static inline void SetHealth(struct RegistryActor *actor, s32 health)
{
    if (health < 0)
        health = 0;
    actor->stats.value = health;
}

static inline void ApplyBoost(struct RegistryActor *actor)
{
    struct RegistryActor *owner = actor;
    if (actor->stats.value > 0 && (gPadState[0] & 0x200)) {
        s32 health;
        actor->stats.boost += 0x1000;
        if (actor->stats.boost > 0x3000)
            actor->stats.boost = 0x3000;
        if (owner->control.boostFrames == 0) {
            health = actor->stats.value - 20;
            if (owner->control.boostEffect < 0)
                owner->control.boostEffect = SpriteSlots_AttachSource(&actor->model.view.parts[0].x);
        } else {
            health = actor->stats.value - 2;
        }
        SetHealth(actor, health);
        owner->control.boostFrames++;
    } else {
        if (owner->control.boostEffect >= 0) {
            TimedSpriteEffect_RequestStop(owner->control.boostEffect);
            owner->control.boostEffect = -1;
        }
        owner->control.boostFrames = 0;
    }
}

void ActorMotion_ApplyInput(struct RegistryActor *actor)
{
    ApplyBoost(actor);
    if (gPadState[0] & 0x100) {
        actor->stats.motion.forward += 0x200;
        if (actor->stats.motion.forward > 0x5000)
            actor->stats.motion.forward = 0x5000;
    }
    if (gPadState[0] & 0x40)
        actor->stats.motion.lateral -= 0x200;
    else if (gPadState[0] & 0x80)
        actor->stats.motion.lateral += 0x200;
    if (actor->stats.motion.vertical >= 0 && (actor->stats.height >> 12) == 0) {
        if (gPadState[0] & 0x10) {
            actor->stats.movementOffset += 0x1000;
            if (actor->stats.movementOffset > 0xC8000)
                actor->stats.movementOffset = 0xC8000;
        } else if (gPadState[0] & 0x20) {
            actor->stats.movementOffset -= 0x1000;
            if (actor->stats.movementOffset < 0x20000)
                actor->stats.movementOffset = 0x20000;
        }
    }
    actor->stats.motion.forward -= 0x20;
    actor->stats.boost -= 0x80;
    if (actor->stats.motion.forward < 0x3000)
        actor->stats.motion.forward = 0x3000;
    if (actor->stats.boost < 0)
        actor->stats.boost = 0;
    actor->stats.motion.lateral = Div(actor->stats.motion.lateral * 90, 100);
}
