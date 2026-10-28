#include "global.h"

#include "scene_actor_control_helpers.h"
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
s32 Callback_InvokeActor(void *, void *);
u32 Trigger_ConsumePending(void *, u32);
void SceneActorControl_StartStoredCurve(struct SceneActorRegistryResetView *);
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
static inline void Fill(void *dst, u32 *tmp, u32 value)
{
    *tmp = value;
    CpuSet(tmp, dst, 0x05000002);
}
struct SavedMotion {
    u8 pad[12];
    struct ContactPosition position;
};
struct WanderActorMotion {
    u32 id;
    u8 unknown4[5];
    u8 direction;
    u8 unknownA[2];
    struct ContactPosition position, motion;
};
struct WanderTriggerView {
    u8 unknown0[0xC];
    u8 trigger[1];
};
static inline u8 Approach(struct SceneActorRegistryResetView *actor, struct SavedMotion *saved,
                          s32 speed)
{
    struct WanderActorMotion *motion = (void *)&actor->id;
    s32 spd = speed;
    s16 dx = saved->position.x - (u16)motion->position.x;
    s16 dz = saved->position.z - (u16)motion->position.z;
    if ((dx < 0 ? -dx : dx) <= 32 && (dz < 0 ? -dz : dz) <= 32)
        return 1;
    if (dx || dz) {
        motion->direction = Math_Atan2(dx, dz);
        motion->motion.x += Shift(gUnk_086149C4[(motion->direction + 64) & 255] * spd, 12);
        motion->motion.z += Shift(gUnk_086149C4[motion->direction] * spd, 12);
    }
    return 0;
}
struct WanderMotionState {
    u8 unknown0[0x3E0];
    u16 timer;
    u8 unknown3E2, mode;
    u8 unknown3E4[0xC];
    struct SavedMotion saved;
    u8 unknown404[0x28];
    struct ContactPosition scratch;
};
struct WanderCallbackView {
    u8 unknown0[0x260];
    void *callback;
};
static inline void SetWanderFlags(struct SceneActorRegistryResetView *actor, u32 bit)
{
    actor->flags2C0 = bit | actor->flags2C0;
}
static inline void Phase(struct SceneActorRegistryResetView *actor, u32 state, u32 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = 0;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
// confidence=low: homes on a stored point, then consumes a pending trigger and restarts.
void SceneActorControl_UpdateWanderTarget(struct SceneActorRegistryResetView *actor)
{
    struct WanderMotionState *body = actor->context;
    u32 zero[1];
    if (Take(&actor->field2B1)) {
        struct WanderMotionState *context = actor->context;
        context->timer = 0;
        context->mode = 1;
        SceneActorControl_StartStoredCurve(actor);
        Control(actor, actor->entryTarget228, 1);
        Fill(&context->scratch, zero, 0);
        SetWanderFlags(actor, 0x800);
    }
    SceneActorControl_TickSway(actor);
    {
        s32 speed = Callback_InvokeActor(actor, ((struct WanderCallbackView *)actor)->callback);
        if (Approach(actor, &body->saved, speed)) {
            if ((u8)sub_08083C38(actor)) {
                Trigger_ConsumePending(((struct WanderTriggerView *)actor)->trigger, 1);
                Phase(actor, 2, 2);
            }
        } else {
            actor->counter2AC++;
        }
    }
}
