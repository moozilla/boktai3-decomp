// MATCHING_CONCESSION: register-pinning
#include "route_sprite_playback.h"
#include "engine.h"
s32 Div(s32, s32);
static inline s32 Truncate12(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}
static inline s32 RandomWith(const u16 *table)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + (u32)table);
}
static inline void Polar(struct ContactPosition *v, s32 angle, s32 radius)
{
    v->x = Truncate12(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = Truncate12(gUnk_086149C4[angle & 255] * radius);
}
/* Picks a random heading and distance, glides toward that point over 20 frames
 * while the animation plays, then records the position and enters state 2. */
void RouteSpriteActor_TickHopToRandomPoint(struct RouteSpritePool *pool,
                                           struct RouteSpriteActor *actor)
{
    if (actor->entered) {
        const u16 *table;
        actor->sprite.flags &= ~1;
        actor->moveTimer = 20;
        actor->animationEnded = 0;
        table = gRandomTable;
        actor->heading = RandomWith(table);
        Polar(&actor->target, actor->heading, (RandomWith(table) & 0xFF) + 0x40);
        {
            struct ContactPosition *t = &actor->target;
            struct ContactPosition *p = &actor->sprite.position;
            t->x = p->x + t->x;
            t->y = p->y + t->y;
            t->z = p->z + t->z;
        }
        actor->frames = 0;
        actor->entered = 0;
    }
    if (actor->moveTimer > 0) {
        actor->sprite.position.x +=
            Div(actor->target.x - actor->sprite.position.x, actor->moveTimer);
        actor->sprite.position.y +=
            Div(actor->elevatedPosition.y - actor->sprite.position.y, actor->moveTimer);
        actor->sprite.position.z +=
            Div(actor->target.z - actor->sprite.position.z, actor->moveTimer);
        actor->moveTimer--;
    }
    if (!actor->animationEnded) {
        u32 ended = RouteSpriteActor_StepPlayback(actor);
        {
            register u32 endedOffset asm("r6") = 0x158;
            asm("" : "+r"(endedOffset));
            {
                register u8 *endedFlag asm("r0") = (u8 *)((u32)actor + endedOffset);
                *endedFlag = ended;
            }
        }
        if (!ended)
            return;
    }
    if (actor->moveTimer <= 0) {
        register u32 offset asm("r6") = 0x148;
        *(struct ContactPosition *)((u32)actor + offset) = actor->sprite.position;
        actor->state = 2;
        {
            register u32 changed asm("r2") = 0x157;
            *(u8 *)((u32)actor + changed) = 1;
            offset += 0x1E;
            {
                register u8 *prepared asm("r1") = (u8 *)((u32)actor + offset);
                *prepared = 1;
            }
            changed -= 1;
            {
                register u8 *entered asm("r1") = (u8 *)((u32)actor + changed);
                *entered = 1;
            }
        }
    }
}
