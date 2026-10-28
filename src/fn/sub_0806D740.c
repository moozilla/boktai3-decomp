// MATCHING_CONCESSION: register-pinning
#include "route_sprite_actor.h"
#include "display_animation.h"
#include "engine.h"
s32 Div(s32, s32);
void RouteSpriteActor_PlanWanderPath(struct RouteSpriteActor *);
static inline s32 Truncate12(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}
static inline s32 Truncate8(s32 value)
{
    return value >= 0 ? value >> 8 : -((-value) >> 8);
}
static inline s32 Truncate6(s32 value)
{
    return value >= 0 ? value >> 6 : -((-value) >> 6);
}
static inline void Polar(struct ContactPosition *v, s32 angle, s32 radius)
{
    v->x = Truncate12(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = Truncate12(gUnk_086149C4[angle & 255] * radius);
}
static inline void SubVec(struct ContactPosition *out, struct ContactPosition *b,
                          struct ContactPosition *a)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}
/* Copy of RouteSpriteActor_StepPlayback taking the sequence separately; the
 * hint in the reverse-hold branch keeps flags live so reload picks r2 there. */
static inline u32 Step2(struct RouteSpriteActor *owner, struct DisplaySequenceState *state)
{
    struct ContactSwarmSprite *sprite = &owner->sprite;
    const volatile u16 *command; /* preserve observed packed-command reads */
    u32 ended, flags, bit;
    u32 duration;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        u32 maskBit = 1;
        bit = maskBit;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->sprite.flags |= 4;
        else
            owner->sprite.flags &= ~4;
    }
    {
        u32 input = state->flags;
        u32 bit;
        register u32 first asm("r1");
        bit = 2;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        asm("" : "+r"(first));
        first = (u8)first;
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->sprite.flags |= 8;
        else
            owner->sprite.flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 temp asm("r1");
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        /* Keep the live mask distinct from its source without binding r7. */
        asm("" : "+&r"(mask) : "r"(temp));
        duration = state->duration;
        elapsed = (u16)elapsed;
        savedDuration = duration;
        if (elapsed >= duration) {
            state->elapsed = 0;
            flags = state->flags;
            if (flags & 4) {
                u32 index = state->index;
                if (index == 0) {
                    u32 hold = 8;
                    hold &= flags;
                    if (hold) {
                        state->elapsed = savedDuration;
                    } else {
                        state->index = state->frameCount - 1;
                        asm("" : : "r"(flags));
                    }
                    ended = 1;
                } else {
                    state->index = index - 1;
                    ended = 0;
                }
            } else {
                u32 index = state->index + 1;
                u32 frameCount;
                u32 endFlag;
                state->index = index;
                frameCount = state->frameCount;
                if ((index & mask) >= frameCount) {
                    endFlag = flags & 8;
                    asm("" : "+r"(endFlag));
                    endFlag = (u8)endFlag;
                    if (endFlag) {
                        state->elapsed = duration;
                        {
                            u32 lastIndex = frameCount - 1;
                            state->index = lastIndex;
                        }
                    } else {
                        state->index = 0;
                    }
                    ended = 1;
                } else {
                    ended = 0;
                }
            }
            {
                u32 offset = state->index << 1;
                const volatile u16 *table;
                table = state->commands;
                command = (const volatile u16 *)((u32)table + offset);
            }
            state->frameWeight = *command & 15;
            {
                register s32 weight asm("r1") = state->frameWeight;
                s32 speed;
                speed = state->speed;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!(u8)speed)
                    state->duration = 1;
            }
        } else {
            ended = 0;
        }
    }
    return ended;
}
/* confidence=low: glides through the 12 spline waypoints from
 * RouteSpriteActor_PlanWanderPath while bobbing; after a full path and the
 * state timer it enters state 3. */
void RouteSpriteActor_TickWandering(struct RouteSpritePool *pool, struct RouteSpriteActor *actor)
{
    struct ContactPosition delta;
    struct RouteSpriteActor *self = actor;
    struct DisplaySequenceState *sequence;
    u32 finished;
    sequence = &actor->sequence;
    finished = 0;
    if (actor->entered) {
        Anim_SetSequence(sequence, pool->sequenceResource, 0xE, 0, finished);
        actor->timer = actor->initialTimer;
        {
            s32 value;
            register u32 angleOffset asm("r5");
            register u8 *angle asm("r1");
            {
                u32 offset;
                register u32 table asm("r3");
                gRandomIndex = (gRandomIndex + 1) & 1023;
                offset = gRandomIndex * 2;
                table = (u32)gRandomTable;
                value = *(const u16 *)(offset + table);
            }
            angleOffset = 0x15A;
            asm("" : "+r"(angleOffset));
            angle = (u8 *)((u32)actor + angleOffset);
            *angle = value;
            {
                u32 targetOffset = 0x140;
                asm("" : "+r"(targetOffset));
                Polar((struct ContactPosition *)((u32)actor + targetOffset), *angle, 0x18);
            }
        }
        actor->moveTimer = 0x460;
        actor->frames = 0;
        actor->entered = 0;
        actor->waypointIndex = 0;
        RouteSpriteActor_PlanWanderPath(actor);
    }
    SubVec(&delta, &actor->sprite.position, &actor->waypoints[actor->waypointIndex]);
    if (actor->moveTimer > 0 && delta.x * delta.x + delta.z * delta.z > 0) {
        s32 speed = Truncate6(actor->moveTimer);
        speed = speed >= 1 ? speed : 1;
        actor->target.x =
            Div(actor->waypoints[actor->waypointIndex].x - actor->sprite.position.x, speed);
        actor->target.z =
            Div(actor->waypoints[actor->waypointIndex].z - actor->sprite.position.z, speed);
        actor->sprite.position.x = actor->sprite.position.x + actor->target.x;
        actor->sprite.position.z = actor->sprite.position.z + actor->target.z;
        actor->moveTimer = actor->moveTimer - actor->frames;
    } else {
        if (++actor->waypointIndex > 12) {
            actor->sprite.position.x = actor->elevatedPosition.x;
            actor->sprite.position.z = actor->elevatedPosition.z;
            RouteSpriteActor_PlanWanderPath(actor);
            actor->moveTimer = 0x460;
            actor->waypointIndex = 0;
            actor->frames = 0;
            finished = 1;
        } else {
            actor->sprite.position.x = actor->waypoints[actor->waypointIndex - 1].x;
            actor->sprite.position.z = actor->waypoints[actor->waypointIndex - 1].z;
        }
    }
    actor->frames++;
    self->sprite.position.y = actor->savedPosition.y + Truncate8(gUnk_086149C4[(u8)actor->angle]);
    actor->savedPosition.x = self->sprite.position.x;
    actor->savedPosition.z = self->sprite.position.z;
    actor->angle += 6;
    Step2(self, sequence);
    if (*(u16 *)&actor->timer)
        (*(u16 *)&actor->timer)--;
    else if (finished) {
        actor->state = 3;
        actor->entered = 1;
    }
}
