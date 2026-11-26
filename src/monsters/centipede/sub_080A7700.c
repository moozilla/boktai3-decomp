#include "scene/motion/scene_actor_chase_reaction.h"
#include "monsters/scene_route_sample.h"
#include "global.h"
#include "monsters/centipede/centipede_body.h"
#include "scene/actor/scene_actor_reaction.h"
#include "scene/motion/scene_actor_terrain.h"
#include "scene/motion/scene_actor_chase.h"

extern const s16 gUnk_086149C4[];

s32 Math_Atan2(s32, s32);
s32 Div(s32, s32);
void CpuSet(const void *, void *, u32);

static inline u8 Take(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}

static inline void State7(struct ChaseActor *a)
{
    u32 behavior = 7;
    a->base.state2A8 = 0;
    a->base.phase2A9 = 0;
    a->base.behavior2AA = behavior;
    a->base.counter2AC = 0;
    a->base.field2B1 = 1;
}
static inline void State1(struct ChaseActor *a)
{
    u32 behavior = 1;
    a->base.state2A8 = 0;
    a->base.phase2A9 = 0;
    a->base.behavior2AA = behavior;
    a->base.counter2AC = 0;
    a->base.field2B1 = 1;
}

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
void sub_080A9DB0(void *, s32, s32);
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
// confidence=low: approaches a saved waypoint and requests a turn when the target heading differs.
void CentipedeBody_ApproachWaypoint(struct ChaseActor *actor)
{
    struct TentacleBody *body = actor->base.context;
    s32 speed;
    struct ContactPosition delta;
    if (Take(&actor->base.field2B1)) {
        body->oscillationDirection = 1;
        body->oscillationPhase = 0;
        SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
        Centipede_ResetOscillation(actor);
    }
    if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque)) {
        State7(actor);
        return;
    }
    speed = Callback_InvokeActor(actor, actor->base.animationQuery.opaque);
    Scale(actor);
    if (Approach(actor, (void *)((u8 *)body + 0x8F8), speed)) {
        s32 angle;
        Centipede_SelectWanderRoute(actor);
        Chase_Delta(&delta, &actor->base.body.position, &body->waypoint);
        actor->base.nextDirection = Math_Atan2(delta.x, delta.z);
        angle = actor->base.nextDirection - actor->base.body.direction;
        if (angle > 128)
            angle -= 256;
        else if (angle < -128)
            angle += 256;
        if (angle < 0)
            angle = -angle;
        if (angle > 32) {
            State1(actor);
            return;
        }
    }
    Centipede_AdvanceMotionPhase(actor, speed);
    {
        u16 *p = &body->routeMotionTimer;
        if (*p) {
            sub_080A9DB0(actor, 18, 0x1000);
            --*p;
        } else
            sub_080A9DB0(actor, 20, 0x6400);
    }
    Centipede_UpdateOscillation(actor, 120, 10);
    actor->base.counter2AC++;
}
