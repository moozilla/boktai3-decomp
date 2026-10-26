#include "global.h"
// MATCHING_CONCESSION: register-pinning
#include "engine.h"
#include "player.h"
#include "contact_actor.h"
void sub_0815E034(struct Player *, struct ContactCollider *);
void SceneActorControl_RequestDeparture(s32);
s32 Player_RequestTimedReset(struct Player *, u32);
void Player_EmitQueuedEffects(struct Player *);
void Player_ApplyContactResponse(struct Player *, struct ContactCollider *,
                                 struct ContactCollider *);
void Player_QueueStatusEffect(struct Player *, s32);
void sub_0813D834(struct Player *, s32, s32);
s32 SceneParticle_CreateDefaultPriority(void *, void *, s32, s32, s32);
extern const s16 gUnk_086149C4[];
static inline u32 specialState(struct Player *p, u32 mode)
{
    u32 active, result;
    if (!p)
        return 0;
    active = p->active;
    result = 0;
    if (active && p->sceneMode == mode)
        result = 1;
    return result;
}
static inline s32 scale(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
static inline u32 playerFlag(struct Player *p, u32 mask)
{
    if (!p)
        return 0;
    {
        u32 *flags = &p->statBonusFlags;
        u32 bits = mask;
        return *flags & bits;
    }
}
union ResponsePosition {
    struct SceneVec point;
    struct {
        u32 x : 16, y : 16, z : 16;
    } words;
};
static inline s32 contactPower(struct ContactCollider *c)
{
    return c->unknown3C;
}
static inline u32 contactFlag(struct ContactCollider *c, u32 mask)
{
    u32 *flags = &c->reactionFlags;
    return *flags & mask;
}
s32 PlayerContact_ApplyResponse(struct Player *p, struct ContactCollider *source,
                                struct ContactCollider *target)
{
    s32 shift;
    s32 damage;
    union ResponsePosition velocity;
    target->cost = 0;
    if (contactPower(source) > 0)
        sub_0815E034(p, source);
    else
        target->cost = 0;
    {
        s32 value = source->responseValue;
        if (value <= 3)
            value = 3;
        target->responseValue = value;
    }
    if (playerFlag(p, 0x40000000))
        shift = -1;
    else
        shift = 0;
    if (((source->reactionFlags & 0x80010)) && !(gEngineFlags & 0x1000))
        p->positionResetState = 1;
    else
        p->positionResetState = 0;
    if (!source->responseParameter)
        p->contactActionTag = 0;
    else {
        p->contactActionTag = 230;
        if (*(s8 *)&p->movementAnimationState >= 0)
            SceneActorControl_RequestDeparture(*(s8 *)&p->movementAnimationState);
        if (specialState(p, 6)) {
            p->sceneMode = 0;
            p->active = 0;
            p->secondaryDrawFlags &= ~0x400;
        }
        if ((contactFlag(source, 0x80000)) && !(playerFlag(p, 0x20000000))) {
            register u32 power asm("r2") = source->unknown3C;
            u32 duration = power + 60;
            u32 flags;
            flags = gEngineFlags & 0x1000;
            // Preserve the original evaluated values without changing the callee's ABI.
            asm volatile("" : "+r"(flags) : "r"(power), "r"(duration));
            Player_RequestTimedReset(p, power + 60);
            *(u16 *)((u8 *)p + 0x4DA) = 1;
        }
        if (!p->positionResetRequested && p->positionResetTimer)
            p->positionResetTimer = 0;
        if (p->tetherMode == 0x16 && !(contactFlag(source, 0x4000))) {
            Player_EmitQueuedEffects(p);
            shift++;
        }
        Player_ApplyContactResponse(p, source, target);
        if (p->actionMode == 1)
            p->contactThresholdPending = 1;
    }
    if (specialState(p, 3)) {
        target->cost = 0;
        damage = 0;
        if (contactFlag(source, 0x40000))
            Player_EmitQueuedEffects(p);
        goto done;
    }
    if ((specialState(p, 5)) || p->tetherMode == 2 || p->actionMode == 3 || p->actionMode == 4)
        shift++;
    if (contactFlag(source, 0x200000)) {
        if (!(target->costFlags & 2)) {
            if (!p->contactSpecial)
                p->contactActionTag = 0x191;
            Player_QueueStatusEffect(p, 1);
        }
    } else if (contactFlag(source, 0x400000)) {
        if (!(target->costFlags & 2)) {
            if (!p->contactSpecial)
                p->contactActionTag = 0x133;
            Player_QueueStatusEffect(p, 2);
        }
    } else if (contactFlag(source, 0x4000)) {
        p->contactActionTag = 0x133;
        Player_QueueStatusEffect(p, 3);
        p->contactSpecial = 0;
    } else if (contactFlag(source, 0x40000))
        Player_EmitQueuedEffects(p);
    if ((contactFlag(source, 0x8000)) && !(playerFlag(p, 0x100)) && !(playerFlag(p, 0x20000000)))
        sub_0813D834(p, 0, 1800);
    if ((contactFlag(source, 0x100000)) && !(playerFlag(p, 0x20000000)))
        sub_0813D834(p, 1, 900);
    if (contactFlag(source, 0x20000)) {
        s32 angle = ((p->facing + 5) & 7) << 5;
        velocity.words.x = scale(gUnk_086149C4[(angle + 64) & 255] * 80);
        velocity.words.y = 200;
        velocity.words.z = scale(gUnk_086149C4[angle] * 80);
        SceneParticle_CreateDefaultPriority(&p->position, &velocity, 0, 0, 1);
    }
    if (shift >= 0)
        damage = target->cost << shift;
    else {
        s32 original = target->cost;
        damage = original >> -shift;
        if (original && !damage)
            damage = 1;
    }
    if (p->particleEmissionCount < damage)
        p->particleEmissionCount = damage;
done:
    return damage;
}
