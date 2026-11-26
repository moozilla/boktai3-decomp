#include "engine/shared/scene_actor_chase_reaction.h"
#include "engine/shared/global.h"
#include "actors/scene_actor_reaction/scene_actor_reaction.h"
#include "engine/shared/scene_actor_terrain.h"
#include "engine/shared/scene_actor_chase.h"

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

static inline s32 Shift(s32 n, u32 bits)
{
    if (n >= 0)
        return n >> bits;
    return -((-n) >> bits);
}

static inline void Control(struct ChaseActor *a, void *cb, u32 kind)
{
    a->base.field2B2 = 1;
    a->base.stepDone = 0;
    a->base.controlKind = kind;
    a->base.target = cb;
    a->base.field2C8 = 0;
    {
        u32 *flags = &a->base.pendingControlFlags;
        *flags &= ~1;
    }
    a->base.pendingControl = 0;
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

struct SavedPair {
    u32 x, z;
};
struct SavedMotion {
    u8 pad[12];
    struct ContactPosition position;
};

void TentacleActor_ResetOscillation(void *);
s32 Callback_InvokeActor(void *, void *);
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);
void TentacleActor_SelectWanderRoute(void *);
void TentacleActor_AdvanceMotionPhase(void *, s32);
void sub_080A9DB0(void *, s32, s32);
void TentacleActor_UpdateOscillation(void *, s32, s32);
static inline u8 Approach(struct ChaseActor *actor, struct SavedMotion *saved, s32 speed)
{
    u8 *motion = (u8 *)actor + 0x50;
    s16 dx = saved->position.x - *(u16 *)(motion + 12);
    s16 dz = saved->position.z - *(u16 *)(motion + 16);
    if ((dx < 0 ? -dx : dx) <= 32 && (dz < 0 ? -dz : dz) <= 32)
        return 1;
    if (dx || dz) {
        motion[9] = Math_Atan2(dx, dz);
        *(u16 *)(motion + 20) += Shift(gUnk_086149C4[(motion[9] + 64) & 255] * speed, 12);
        *(u16 *)(motion + 24) += Shift(gUnk_086149C4[motion[9]] * speed, 12);
    }
    return 0;
}
static inline void Scale(struct ChaseActor *actor)
{
    u16 *scale = SceneActorChase_PrepareAnimationScale(actor, 0xA02F);
    if (actor->base.drawableKind == 0)
        Anim_SetDurationScale((u8 *)actor->base.drawable + 0x48, *scale);
    else
        Anim_SetFrameDurationScale((u8 *)actor->base.drawable + 0x20, *scale);
}
// confidence=low: approaches a saved waypoint and requests a turn when the target heading differs.
void TentacleBody_ApproachWaypoint(struct ChaseActor *actor)
{
    u8 *body = actor->base.context;
    s32 speed;
    struct ContactPosition delta;
    if (Take(&actor->base.field2B1)) {
        u8 *flag = body + 0x8D4;
        u32 zero = 0;
        u32 one = 1;
        *flag = one;
        *(u16 *)(body + 0x8D8) = zero;
        {
            void *cb = *(void **)((u8 *)actor + 0x228);
            actor->base.field2B2 = one;
            actor->base.stepDone = 0;
            actor->base.controlKind = one;
            actor->base.target = cb;
            actor->base.field2C8 = zero;
            {
                u32 *flags = &actor->base.pendingControlFlags;
                u32 mask = ~1;
                *flags &= mask;
            }
            actor->base.pendingControl = zero;
        }
        TentacleActor_ResetOscillation(actor);
    }
    if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque)) {
        State7(actor);
        return;
    }
    speed = Callback_InvokeActor(actor, actor->base.animationQuery.opaque);
    Scale(actor);
    if (Approach(actor, (void *)(body + 0x8F8), speed)) {
        s32 angle;
        TentacleActor_SelectWanderRoute(actor);
        Chase_Delta(&delta, &actor->base.body.position, (void *)(body + 0x904));
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
    TentacleActor_AdvanceMotionPhase(actor, speed);
    {
        u16 *p = (void *)(body + 0x8E4);
        if (*p) {
            sub_080A9DB0(actor, 18, 0x1000);
            --*p;
        } else
            sub_080A9DB0(actor, 20, 0x6400);
    }
    TentacleActor_UpdateOscillation(actor, 120, 10);
    actor->base.counter2AC++;
}
