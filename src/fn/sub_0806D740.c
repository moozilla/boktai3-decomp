#include "route_sprite_actor.h"
#include "display_animation.h"
#include "engine.h"
#include "player_cue_sequence.h"
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
    return PlayerCue_StepSequenceEnded(&owner->sprite, state);
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
        gRandomIndex = (gRandomIndex + 1) & 1023;
        actor->angle = ((const u16 *)RANDOM_TABLE_ADDR)[gRandomIndex];
        Polar(&actor->target, (u8)actor->angle, 0x18);
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
