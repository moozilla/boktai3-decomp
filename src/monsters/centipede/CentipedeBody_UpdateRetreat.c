#include "scene/motion/scene_actor_chase_reaction.h"
#include "monsters/scene_route_sample.h"
#include "global.h"
#include "monsters/centipede/centipede_body.h"
#include "scene/actor/scene_actor_reaction.h"
#include "scene/motion/scene_actor_terrain.h"
#include "engine/engine.h"
#include "player/player.h"
#include "engine/ram.h"
#include "scene/motion/scene_actor_chase.h"
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
s32 Div(s32, s32);
void CpuSet(const void *, void *, u32);
void Bee_BuildMovementCurve(void *, void *);
void Bee_SubVector(void *, void *, void *);
s32 Bee_GetMovementVariant(void *);
static inline u8 Take(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
static inline u8 Test(u32 v, u32 m)
{
    if (v & m)
        return 1;
    return 0;
}
static inline u32 Rand(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return gRandomTable[gRandomIndex];
}
static inline void Polar(struct ContactPosition *out, u32 angle, s32 speed)
{
    out->x = SceneRoute_ScaleComponent(gUnk_086149C4[(angle + 64) & 255] * speed);
    out->y = 0;
    out->z = SceneRoute_ScaleComponent(gUnk_086149C4[angle & 255] * speed);
}
static inline void Add(struct ContactPosition *out, const struct ContactPosition *from)
{
    out->x = from->x + out->x;
    out->y = out->y + from->y;
    out->z = from->z + out->z;
}
static inline void State(struct ChaseActor *a)
{
    u32 behavior = 8;
    a->base.state2A8 = 0;
    a->base.phase2A9 = 0;
    a->base.behavior2AA = behavior;
    a->base.counter2AC = 0;
    a->base.field2B1 = 1;
}

struct SavedPair {
    u32 x, z;
};
struct SavedMotion {
    u8 pad[12];
    struct ContactPosition position;
};

void Centipede_ResetOscillation(void *);
s32 Callback_InvokeActor(void *, void *);
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);
void Centipede_SelectWanderRoute(void *);
void Centipede_AdvanceMotionPhase(void *, s32);
void Centipede_UpdateSegmentPositions(void *, s32, s32);
void Centipede_UpdateOscillation(void *, s32, s32);
static inline u8 Approach(struct ChaseActor *actor, struct SavedMotion *saved, s32 speed)
{
    struct SceneActorBody *body = &actor->base.body;
    s16 dx = saved->position.x - body->position.x;
    s16 dz = saved->position.z - body->position.z;
    if ((dx < 0 ? -dx : dx) <= 32 && (dz < 0 ? -dz : dz) <= 32)
        return 1;
    if (dx || dz) {
        body->direction = Math_Atan2(dx, dz);
        body->motion.x +=
            SceneRoute_ScaleComponent(gUnk_086149C4[(body->direction + 64) & 255] * speed);
        body->motion.z += SceneRoute_ScaleComponent(gUnk_086149C4[body->direction] * speed);
    }
    return 0;
}
static inline void Scale(struct ChaseActor *actor)
{
    u16 *scale = SceneActorChase_PrepareAnimationScale(actor, 0xA02F);
    if (actor->base.drawableKind == 0)
        Anim_SetDurationScale(&((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence, *scale);
    else
        Anim_SetFrameDurationScale(&((struct SceneActorAnimationTileDrawable *)actor->base.drawable)->tile, *scale);
}
// confidence=low: moves toward a saved waypoint using the facing quadrant behind the actor.
void CentipedeBody_UpdateRetreat(struct ChaseActor *actor)
{
    struct TentacleBody *body = actor->base.context;
    s32 speed;
    struct ContactPosition delta;
    if (Take(&actor->base.field2B1)) {
        SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
        Centipede_ResetOscillation(actor);
        {
            u32 direction = (((actor->base.tracking.heading) + 128) & 255) >> 6;
            body->wanderDirection = direction;
        }
        body->wanderAttempts = 0;
        Centipede_SelectWanderRoute(actor);
    }

    if (body->hitReactionTimer <= 0)
        body->hitReactionTimer = 1;
    speed = Callback_InvokeActor(actor, actor->base.animationQuery.opaque);
    Scale(actor);
    if (Approach(actor, (void *)((u8 *)body + 0x8F8), speed)) {
        Centipede_SelectWanderRoute(actor);
        Chase_Delta(&delta, &actor->base.body.position, &body->waypoint);
        actor->base.nextDirection = Math_Atan2(delta.x, delta.z);
    }
    Centipede_AdvanceMotionPhase(actor, speed);
    {
        u16 *p = &body->routeMotionTimer;
        if (*p) {
            Centipede_UpdateSegmentPositions(actor, 18, 0x1000);
            --*p;
        } else
            Centipede_UpdateSegmentPositions(actor, 20, 0x6400);
    }
    Centipede_UpdateOscillation(actor, 120, 10);
    if ((s32)actor->base.counter2AC > 80) {
        State(actor);
    } else
        actor->base.counter2AC++;
}
