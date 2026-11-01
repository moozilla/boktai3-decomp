#include "global.h"

#include "scene_actor_control_helpers.h"
#include "engine.h"
#include "player.h"
#include "ram.h"
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
s32 Callback_InvokeActor(void *, void *);
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
static inline u32 Rand(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return *(const u16 *)((index << 1) + (u32)gRandomTable);
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
static inline u8 Approach(struct SceneActorRegistryResetView *actor, struct SavedMotion *saved,
                          s32 inSpeed)
{
    u8 *motion = (u8 *)actor + 0x50;
    s32 speed = inSpeed;
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
// confidence=low: homes on a stored point, then picks a new point around the player.
void SceneActorControl_WanderAroundPlayer(struct SceneActorRegistryResetView *actor)
{
    u8 *body = actor->context;
    u32 zero[1];
    if (Take(&actor->field2B1)) {
        u8 *context = actor->context;
        Control(actor, actor->entryTarget228, 1);
        Fill(context + 0x42C, zero, 0);
        context[0x3E3] = 1;
    }
    SceneActorControl_TickSway(actor);
    if (Approach(actor, (void *)(body + 0x3F0),
                 Callback_InvokeActor(actor, *(void **)((u8 *)actor + 0x260)))) {
        if ((u8)SceneActorControl_AdvancePositionCurve(actor)) {
            struct ContactPosition target;
            u32 angle;
            angle = (u8)Rand();
            target = *(struct ContactPosition *)(((u8 **)gPlayers)[0] + 0x30);
            target.x += Shift(gUnk_086149C4[angle], 4);
            target.z += Shift(gUnk_086149C4[(angle + 64) & 255], 4);
            SceneActorControl_StartPositionCurve(actor, &target);
            {
                u32 *flags = &((struct SceneActorControlMotionState *)actor->context)->flags3D4;
                u32 mask = ~8;
                *flags &= mask;
            }
        }
    } else {
        actor->counter2AC++;
    }
}
