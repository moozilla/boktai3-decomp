#include "global.h"

#include "scene_actor_control_helpers.h"
#include "engine.h"
#include "player.h"
#include "ram.h"
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
s32 Callback_InvokeActor(void *, void *);
void CpuSet(const void *, void *, u32);
void SceneActorControl_UpdateTimedFacing(struct SceneActorRegistryResetView *);
void SceneActorControl_ResetCollisionPhase(struct SceneActorRegistryResetView *);
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
static inline void Control(struct SceneActorRegistryResetView *a, void *cb, u32 kind)
{
    u32 zero;
    a->field2B2 = 1;
    zero = 0;
    a->field2B0 = zero;
    a->field2B5 = kind;
    a->target = cb;
    a->field2C8 = zero;
    {
        u32 *flags = &a->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    a->word104 = zero;
}
static inline void Phase(struct SceneActorRegistryResetView *actor, u32 behavior)
{
    actor->state2A8 = 0;
    actor->phase2A9 = 0;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline u32 Rand(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return *(const u16 *)((index << 1) + RANDOM_TABLE_ADDR);
}
static inline void Fill(void *dst, u32 *tmp, u32 value)
{
    *tmp = value;
    CpuSet(tmp, dst, 0x05000002);
}
struct SavedMotion {
    u8 pad[12];
    struct ContactPosition position;
};
struct StoredMotionState {
    u8 unknown0[0x3D4];
    u32 flags;
    u8 unknown3D8[0xB];
    u8 mode;
    u8 unknown3E4[4];
    u16 timer;
    u8 unknown3EA[6];
    struct SavedMotion saved;
    u8 unknown404[0x28];
    struct ContactPosition scratch;
};
struct ActorMotionView {
    u32 id;
    u8 unknown4[5];
    u8 direction;
    u8 unknownA[2];
    struct ContactPosition position, motion;
};
struct PlayerPositionView {
    u8 unknown0[0x30];
    struct ContactPosition position;
};
struct ActorApproachCallbackView {
    u8 unknown0[0x260];
    void *callback;
};
static inline u8 Approach(struct SceneActorRegistryResetView *actor, struct SavedMotion *saved,
                          s32 inSpeed)
{
    struct ActorMotionView *motion = (void *)&actor->body.id;
    s32 speed = inSpeed;
    s16 dx = saved->position.x - (u16)motion->position.x;
    s16 dz = saved->position.z - (u16)motion->position.z;
    if ((dx < 0 ? -dx : dx) <= 32 && (dz < 0 ? -dz : dz) <= 32)
        return 1;
    if (dx || dz) {
        motion->direction = Math_Atan2(dx, dz);
        motion->motion.x += Shift(gUnk_086149C4[(motion->direction + 64) & 255] * speed, 12);
        motion->motion.z += Shift(gUnk_086149C4[motion->direction] * speed, 12);
    }
    return 0;
}
// confidence=low: waits a timed interval, homes on a stored point, then picks a new point.
void sub_080844B8(struct SceneActorRegistryResetView *actor)
{
    struct StoredMotionState *body = actor->context;
    u32 zero[1];
    if (Take(&actor->field2B1)) {
        Control(actor, (void *)SceneActorControl_UpdateTimedFacing, 0x24);
        Fill(&body->scratch, zero, 0);
        body->mode = 1;
        body->timer = 0;
        {
            u32 angle;
            struct ContactPosition target;
            angle = (u8)Rand();
            target = ((struct PlayerPositionView *)gPlayers[0])->position;
            target.x += Shift(gUnk_086149C4[angle], 3);
            target.z += Shift(gUnk_086149C4[(angle + 64) & 255], 3);
            SceneActorControl_InitPositionCurveEndpoints(actor, &target);
            SceneActorControl_AdvancePositionCurve(actor);
            {
                u32 *flags = &body->flags;
                u32 mask = ~8;
                *flags &= mask;
            }
        }
    }
    if (body->timer != 0) {
        u32 timer = body->timer;
        timer--;
        body->timer = timer;
        if ((timer << 16) == 0) {
            SceneActorControl_StartPositionCurve(
                actor, &((struct PlayerPositionView *)gPlayers[0])->position);
            {
                u32 mask = 8;
                u32 *flags = &body->flags;
                *flags |= mask;
            }
        }
    } else {
        SceneActorControl_TickSway(actor);
        if (Approach(actor, &body->saved,
                     Callback_InvokeActor(actor,
                                          ((struct ActorApproachCallbackView *)actor)->callback))) {
            if ((u8)SceneActorControl_AdvancePositionCurve(actor)) {
                struct ContactPosition target;
                u32 angle;
                angle = (u8)Rand();
                target = ((struct PlayerPositionView *)gPlayers[0])->position;
                target.x += Shift(gUnk_086149C4[angle], 3);
                target.z += Shift(gUnk_086149C4[(angle + 64) & 255], 3);
                SceneActorControl_InitPositionCurveEndpoints(actor, &target);
                SceneActorControl_AdvancePositionCurve(actor);
            }
        }
        if (actor->tracking.distance > 0xFDE8) {
            SceneActorControl_ResetCollisionPhase(actor);
            actor->body.direction = actor->tracking.heading;
            body->timer = 0x1E;
            return;
        }
    }
    if (actor->field2B0) {
        Phase(actor, 4);
    } else {
        actor->counter2AC++;
    }
}
