#include "global.h"
#include "ram.h"
#include "scene_pool.h"
#include "actor_stats.h"
#include "spatial_index.h"
#include "contact_actor.h"

/* Clear the singleton sprite drop manager pointer. */

void ActorRecoil_ReturnZero(void)
{
    gCompactParticlePool = 0;
}

static inline u8 ScenePool_HasControlRestriction(struct ScenePoolActor *actor, u32 mask)
{
    if (actor->flags5ee & mask)
        return 1;
    return 0;
}

static inline u32 ScenePool_TestPlayerFlags(u32 flags, u32 mask)
{
    return flags & mask;
}

u8 ScenePool_CanAcceptControl(struct ScenePoolActor *actor)
{
    if ((actor->linkedPlayer && ScenePool_TestPlayerFlags(actor->linkedPlayer->flags, 4)) ||
        ScenePool_HasControlRestriction(actor, 4))
        return 0;
    return 1;
}

// confidence=low: body-volume callback applies knockback and records special contacts.

extern const s16 gUnk_086149C4[];

s32 Math_Atan2(s32, s32);

static inline u8 Recoil_TestDamageFlags(u32 mask, struct ActorStats *obj, u8 **out)
{
    u8 *flags = &obj->damageFlags;
    u32 test = *flags & mask;
    *out = flags;
    if (test)
        return TRUE;
    return FALSE;
}

static inline void Recoil_SetDamageFlags(u8 bits, u8 *flags)
{
    *flags = bits | *flags;
}

static inline bool32 ActorRecoil_IsDamageCategory(s32 kind, s32 want)
{
    if (kind == want)
        return TRUE;
    return FALSE;
}

static inline s32 ActorRecoil_ScaleVelocity(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

static inline void ActorRecoil_SetPolarVelocity(struct ContactPosition *v, s32 angle, s32 speed)
{
    v->x = ActorRecoil_ScaleVelocity(gUnk_086149C4[(angle + 64) & 255] * speed);
    v->y = 0;
    v->z = ActorRecoil_ScaleVelocity(gUnk_086149C4[angle & 255] * speed);
}

void SceneMotion_PlayKindSound(struct SpatialEntry *);

s32 ModifierStats_ApplyKind15(struct ScenePoolActor *, s32);

void ActorRecoil_ReceiveBodyContact(void *owner, struct SpatialEntry *other)
{
    struct SpatialEntry *msg = other;
    struct ActorStats *obj = owner;
    struct {
        struct ScenePoolVec32 target;
        struct ScenePoolVec32 self;
        struct ScenePoolVec32 diff;
    } positions;
    struct ContactPosition vel;
    s32 angle;
    s32 durationScale;
    u8 *flags;

    if (!ScenePool_CanAcceptControl((struct ScenePoolActor *)obj) || msg->mask == 7) {
        obj->damageTimer = 0;
        return;
    }
    if (Recoil_TestDamageFlags(1, obj, &flags))
        goto sound;
    if (msg->mask == 7) {
        obj->damageTimer = 0;
        goto sound;
    }
    if (ActorRecoil_IsDamageCategory(msg->category, 4) ||
        ActorRecoil_IsDamageCategory(msg->category, 3)) {
        obj->damageTimer = 0;
        goto sound;
    }
    obj->pendingDamage = msg->kind;
    obj->damageType = msg->mask;
    Recoil_SetDamageFlags(1, flags);
    positions.self.x = obj->contactPosition.x + obj->contactOffset.x;
    positions.self.y = obj->contactPosition.y + obj->contactOffset.y;
    positions.self.z = obj->contactPosition.z + obj->contactOffset.z;
    positions.target.x = msg->position.x + msg->offset.x;
    positions.target.y = msg->position.y + msg->offset.y;
    positions.target.z = msg->position.z + msg->offset.z;
    positions.diff.x = positions.self.x - positions.target.x;
    positions.diff.y = positions.self.y - positions.target.y;
    positions.diff.z = positions.self.z - positions.target.z;
    angle = Math_Atan2(positions.diff.x, positions.diff.z);
    angle = (angle >> 5) << 5;
    ActorRecoil_SetPolarVelocity(&vel, angle, msg->parameter);
    durationScale = obj->secondaryConfig[0] >> 1;
    obj->knockback.forward = vel.x << 10;
    obj->knockback.lateral = vel.z << 10;
    obj->knockbackTimer = 60 - durationScale;
    if (obj->knockbackTimer < 10)
        obj->knockbackTimer = 10;
    if ((u32)(angle - 10) > 0xEC) {
        obj->motion.forward += 0x3000;
    } else {
        obj->motion.forward -= 0x100;
        obj->boost = 0;
    }
sound:
    SceneMotion_PlayKindSound(msg);
    if (ActorRecoil_IsDamageCategory(msg->category, 4) ||
        ActorRecoil_IsDamageCategory(msg->category, 3)) {
        obj->increment = ModifierStats_ApplyKind15((struct ScenePoolActor *)obj, msg->kind);
        obj->mode = msg->category;
        obj->pending = 1;
    }
}

extern const s16 gUnk_086149C4[];

static inline u8 Recoil_IsIgnored(s32 type)
{
    u8 r = 0;
    if (type == 2)
        r = 1;
    return r;
}

static inline struct ActorStats *ActorRecoil_GetStats(void *owner)
{
    return owner;
}

void ActorRecoil_ApplyDirectionalImpulse(void *owner, struct SpatialEntry *msg)
{
    struct ScenePoolVec32 src, dst, diff;
    struct ContactPosition vel;
    s32 angle;
    s32 a, b, c, d;
    if (!ScenePool_CanAcceptControl(owner))
        return;
    if (Recoil_IsIgnored(msg->category))
        return;
    a = ActorRecoil_GetStats(owner)->contactPosition.x +
        ActorRecoil_GetStats(owner)->contactOffset.x;
    dst.x = a;
    b = ActorRecoil_GetStats(owner)->contactPosition.z +
        ActorRecoil_GetStats(owner)->contactOffset.z;
    dst.z = b;
    c = msg->position.x + msg->offset.x;
    src.x = c;
    d = msg->position.z + msg->offset.z;
    src.z = d;
    diff.x = a - c;
    diff.z = b - d;
    angle = Math_Atan2(a - c, b - d);
    angle = (angle >> 5) << 5;
    ActorRecoil_SetPolarVelocity(&vel, angle, 8);
    ActorRecoil_GetStats(owner)->knockback.forward = vel.x << 10;
    ActorRecoil_GetStats(owner)->knockback.lateral = vel.z << 10;
    if ((u32)(angle - 10) > 0xEC) {
        ActorRecoil_GetStats(owner)->motion.forward += 0x3000;
        ActorRecoil_GetStats(owner)->knockbackTimer = 10;
    } else {
        ActorRecoil_GetStats(owner)->motion.forward -= 0x2000;
        ActorRecoil_GetStats(owner)->boost = 0;
        ActorRecoil_GetStats(owner)->knockbackTimer = 15;
    }
}

// Tentative family and semantic names; layouts follow observed field offsets.

struct RecoilCollisionActor {
    u8 unknown0[0x18];
    u8 owner[0x544 - 0x18];
    struct SpatialEntry hitVolume;
    struct SpatialEntry bodyVolume;
    u8 unknown5C4[0x6C];
    s16 width, height;
    u8 unknown634[0x8f8 - 0x634];
    s8 group;
};

void ActorRecoil_InitCollisionVolumes(struct RecoilCollisionActor *p)
{
    struct ContactPackedVector position, extents, relativeOffset;
    void *owner = p->owner;
    s32 group = p->group + 1;
    position.x = p->width;
    position.y = p->height - 96;
    position.z = p->height - 96;
    extents.x = 22;
    extents.y = 10;
    extents.z = 4;
    relativeOffset.x = 0;
    relativeOffset.y = -extents.y;
    relativeOffset.z = 2;
    SpatialIndex_InitEntry(&p->bodyVolume, (struct ContactPosition *)&position,
                           (struct ContactPosition *)&extents,
                           (struct ContactPosition *)&relativeOffset, 20, group, 0, 0, 12,
                           ActorRecoil_ReceiveBodyContact, owner);
    position.x = p->width;
    position.y = p->height - 96;
    position.z = p->height - 96;
    extents.x = 16;
    extents.y = 8;
    extents.z = 6;
    relativeOffset.x = 0;
    relativeOffset.y = -extents.y;
    relativeOffset.z = 4;
    SpatialIndex_InitEntry(&p->hitVolume, (struct ContactPosition *)&position,
                           (struct ContactPosition *)&extents,
                           (struct ContactPosition *)&relativeOffset, 44, group, 0, 15, 12,
                           ActorRecoil_ApplyDirectionalImpulse, owner);
    SpatialIndex_InsertEntry(&p->bodyVolume);
}

// Tentative family and semantic names; layouts follow observed field offsets.

struct RecoilMotionActor {
    struct Task task;
    struct ScenePoolActor body;
    u8 collisionInfo[1];
};

s32 MotionTween_Advance(struct ScenePoolActor *, void *);

s32 Callback_InvokeActor(void *, void *);

void ScenePool_ClampForwardBias(struct ScenePoolActor *);

s32 Div(s32, s32);

u8 LinkParticipant_IsLocallySelected(struct RecoilMotionActor *);

void Character_UpdateLoopingSound(struct ScenePoolActor *);

s32 ActorRecoil_UpdateMotion(struct RecoilMotionActor *p)
{
    struct ScenePoolActor *body = &p->body;
    s32 result = MotionTween_Advance(body, p->collisionInfo);
    s32 changed = 0;
    if (Callback_InvokeActor(body, body->drain.callback)) {
        if (body->requestedSound == 0x89)
            body->requestedSound = 0x8b;
        else
            body->requestedSound = 0x8a;
        changed = 1;
    }
    ScenePool_ClampForwardBias(body);
    if (!body->airborne && !result) {
        body->forwardSpeed -= 0x20;
        if (!changed)
            body->requestedSound = 0x87;
    }
    body->lateralSpeed = Div(body->lateralSpeed * 9, 10);
    if (LinkParticipant_IsLocallySelected(p))
        Character_UpdateLoopingSound(body);
    return result;
}
