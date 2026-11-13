#include "global.h"
#include "ram.h"
#include "actor_recoil.h"

/* CompactParticlePool_Clear: Clears the singleton sprite drop manager pointer */

void CompactParticlePool_Clear(void)
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

static inline s32 ScaleMotion(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

static inline u8 IsContactBusy(struct ScenePoolActor *actor, u8 **out)
{
    u32 mask = 1;
    u8 *flags = &actor->flags5ee;
    u32 test = *flags & mask;
    *out = flags;
    if (test)
        return TRUE;
    return FALSE;
}

static inline bool32 IsContactKind(s32 kind, s32 want)
{
    if (kind == want)
        return TRUE;
    return FALSE;
}

static inline void SetContactVelocity(struct SceneVec *v, s32 angle, s32 speed)
{
    v->x = ScaleMotion(gUnk_086149C4[(angle + 64) & 255] * speed);
    v->y = 0;
    v->z = ScaleMotion(gUnk_086149C4[angle & 255] * speed);
}

void ActorRecoil_ReceiveBodyContact(struct ScenePoolActor *actorInput,
                                    struct ScenePoolContact *contactInput)
{
    struct ScenePoolContact *contact = contactInput;
    struct ScenePoolActor *actor = actorInput;
    struct {
        struct ActorRecoilVector target;
        struct ActorRecoilVector self;
        struct ActorRecoilVector diff;
    } positions;
    struct SceneVec velocity;
    s32 angle;
    s32 resistance;
    u8 *flags;

    if (!ScenePool_CanAcceptControl(actor) || contact->mask == 7) {
        actor->bodyContact.contactState = 0;
        return;
    }
    if (IsContactBusy(actor, &flags))
        goto sound;
    if (contact->mask == 7) {
        actor->bodyContact.contactState = 0;
        goto sound;
    }
    if (IsContactKind(contact->kind, 4) || IsContactKind(contact->kind, 3)) {
        actor->bodyContact.contactState = 0;
        goto sound;
    }
    actor->unknown68 = contact->value;
    actor->contactType = contact->mask;
    {
        u8 one = 1;
        *flags |= one;
    }
    positions.self.x = actor->bodyContact.position.x + actor->bodyContact.offset.x;
    positions.self.y = actor->bodyContact.position.y + actor->bodyContact.offset.y;
    positions.self.z = actor->bodyContact.position.z + actor->bodyContact.offset.z;
    positions.target.x = contact->position.x + contact->offset.x;
    positions.target.y = contact->position.y + contact->offset.y;
    positions.target.z = contact->position.z + contact->offset.z;
    positions.diff.x = positions.self.x - positions.target.x;
    positions.diff.y = positions.self.y - positions.target.y;
    positions.diff.z = positions.self.z - positions.target.z;
    angle = Math_Atan2(positions.diff.x, positions.diff.z);
    angle = (angle >> 5) << 5;
    SetContactVelocity(&velocity, angle, contact->speed);
    resistance = actor->recoilResistance >> 1;
    actor->recoilX = velocity.x << 10;
    actor->recoilZ = velocity.z << 10;
    actor->motionLock = 60 - resistance;
    if (actor->motionLock < 10)
        actor->motionLock = 10;
    if ((u32)(angle - 10) > 0xEC) {
        actor->forwardSpeed += 0x3000;
    } else {
        actor->forwardSpeed -= 0x100;
        actor->forwardBias = 0;
    }
sound:
    SceneMotion_PlayKindSound(contact);
    if (IsContactKind(contact->kind, 4) || IsContactKind(contact->kind, 3)) {
        actor->contactValue = ModifierStats_ApplyKind15(actor, contact->value);
        actor->contactKind = contact->kind;
        actor->contactPending = 1;
    }
}

static inline u8 Recoil_IsIgnored(s32 type)
{
    u8 r = 0;
    if (type == 2)
        r = 1;
    return r;
}

static inline void Recoil_SetVelocity(struct ActorRecoilVelocity *out, s32 angle, s32 k)
{
    out->x = ScaleMotion(gUnk_086149C4[(angle + 0x40) & 0xff] * k);
    out->y = 0;
    out->z = ScaleMotion(gUnk_086149C4[angle & 0xff] * k);
}

void ActorRecoil_ApplyDirectionalImpulse(struct ScenePoolActor *actor,
                                         struct ScenePoolContact *contact)
{
    struct ActorRecoilVector source, destination, difference;
    struct ActorRecoilVelocity velocity;
    s32 angle;
    s32 actorX, actorZ, contactX, contactZ;
    if (!ScenePool_CanAcceptControl(actor))
        return;
    if (Recoil_IsIgnored(contact->kind))
        return;
    actorX = actor->bodyContact.position.x + actor->bodyContact.offset.x;
    destination.x = actorX;
    actorZ = actor->bodyContact.position.z + actor->bodyContact.offset.z;
    destination.z = actorZ;
    contactX = contact->position.x + contact->offset.x;
    source.x = contactX;
    contactZ = contact->position.z + contact->offset.z;
    source.z = contactZ;
    difference.x = actorX - contactX;
    difference.z = actorZ - contactZ;
    angle = Math_Atan2(actorX - contactX, actorZ - contactZ);
    angle = (angle >> 5) << 5;
    Recoil_SetVelocity(&velocity, angle, 8);
    actor->recoilX = velocity.x << 10;
    actor->recoilZ = velocity.z << 10;
    if ((u32)(angle - 10) > 0xEC) {
        actor->forwardSpeed += 0x3000;
        actor->motionLock = 10;
    } else {
        actor->forwardSpeed -= 0x2000;
        actor->forwardBias = 0;
        actor->motionLock = 15;
    }
}

void ActorRecoil_InitCollisionVolumes(struct ActorRecoil *actor)
{
    union ActorRecoilPoint pos, size, offset;
    void *owner = &actor->body;
    s32 group = actor->group;
    {
        s32 one = 1;
        group += one;
    }
    pos.packed.x = actor->body.collisionWidth;
    pos.packed.y = actor->body.collisionHeight - 96;
    pos.packed.z = actor->body.collisionHeight - 96;
    size.packed.x = 22;
    size.packed.y = 10;
    size.packed.z = 4;
    offset.packed.x = 0;
    offset.packed.y = -size.packed.y;
    offset.packed.z = 2;
    SpatialIndex_InitEntry(&actor->body.bodyContact, &pos, &size, &offset, 20, group, 0, 0, 12,
                           ActorRecoil_ReceiveBodyContact, owner);
    pos.value.x = actor->body.collisionWidth;
    pos.value.y = actor->body.collisionHeight - 96;
    pos.value.z = actor->body.collisionHeight - 96;
    size.value.x = 16;
    size.value.y = 8;
    size.value.z = 6;
    offset.value.x = 0;
    offset.value.y = -size.value.y;
    offset.value.z = 4;
    SpatialIndex_InitEntry(&actor->body.attackContact, &pos, &size, &offset, 44, group, 0, 15, 12,
                           ActorRecoil_ApplyDirectionalImpulse, owner);
    SpatialIndex_InsertEntry(&actor->body.bodyContact);
}

s32 ActorRecoil_UpdateMotion(struct ActorRecoil *actor)
{
    struct ScenePoolActor *body = &actor->body;
    s32 result = MotionTween_Advance(body, actor->collisionInfo);
    s32 changed = 0;
    if (Callback_InvokeActor(body, body->contactCallback)) {
        if (body->stateSound == 0x89)
            body->stateSound = 0x8b;
        else
            body->stateSound = 0x8a;
        changed = 1;
    }
    ScenePool_ClampForwardBias(body);
    if (!body->contactActive && !result) {
        body->forwardSpeed -= 0x20;
        if (!changed)
            body->stateSound = 0x87;
    }
    body->lateralSpeed = Div(body->lateralSpeed * 9, 10);
    if (LinkParticipant_IsLocallySelected(actor))
        Character_UpdateLoopingSound(body);
    return result;
}
