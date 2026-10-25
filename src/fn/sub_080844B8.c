#include "global.h"

#include "scene_actor_control_helpers.h"
extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
extern const s16 gUnk_086149C4[];
extern u8 *gUnk_02000580[];
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
static inline u32 Rand(void)
{
    u32 index = (gUnk_03005308 + 1) & 1023;
    gUnk_03005308 = index;
    return *(const u16 *)((index << 1) + (u32)gUnk_0203B400);
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
#ifdef NONMATCHING
// fuzzy: 94.0%
// scope shape 35 (differences once low registers are set aside)
// confidence=low: waits a timed interval, homes on a stored point, then picks a new point.
void sub_080844B8(struct SceneActorRegistryResetView *actor)
{
    u8 *body = actor->context;
    u32 zero[1];
    if (Take(&actor->field2B1)) {
        Control(actor, (void *)SceneActorControl_UpdateTimedFacing, 0x24);
        Fill(body + 0x42C, zero, 0);
        body[0x3E3] = 1;
        *(u16 *)(body + 0x3E8) = 0;
        {
            u32 angle;
            struct ContactPosition target;
            angle = (u8)Rand();
            target = *(struct ContactPosition *)(gUnk_02000580[0] + 0x30);
            target.x += Shift(gUnk_086149C4[angle], 3);
            target.z += Shift(gUnk_086149C4[(angle + 64) & 255], 3);
            SceneActorControl_InitPositionCurveEndpoints(actor, &target);
            sub_08083C38(actor);
            {
                u32 *flags = (u32 *)(body + 0x3D4);
                u32 mask = ~8;
                *flags &= mask;
            }
        }
    }
    if (*(u16 *)(body + 0x3E8) != 0) {
        u32 timer = *(u16 *)(body + 0x3E8);
        timer--;
        *(u16 *)(body + 0x3E8) = timer;
        if ((timer << 16) == 0) {
            SceneActorControl_StartPositionCurve(
                actor, (struct ContactPosition *)(gUnk_02000580[0] + 0x30));
            {
                u32 mask = 8;
                u32 *flags = (u32 *)(body + 0x3D4);
                *flags |= mask;
            }
        }
    } else {
        SceneActorControl_TickSway(actor);
        if (Approach(actor, (void *)(body + 0x3F0),
                     Callback_InvokeActor(actor, *(void **)((u8 *)actor + 0x260)))) {
            if ((u8)sub_08083C38(actor)) {
                struct ContactPosition target;
                u32 angle;
                angle = (u8)Rand();
                target = *(struct ContactPosition *)(gUnk_02000580[0] + 0x30);
                target.x += Shift(gUnk_086149C4[angle], 3);
                target.z += Shift(gUnk_086149C4[(angle + 64) & 255], 3);
                SceneActorControl_InitPositionCurveEndpoints(actor, &target);
                sub_08083C38(actor);
            }
        }
        if (actor->distanceF0 > 0xFDE8) {
            SceneActorControl_ResetCollisionPhase(actor);
            actor->direction = actor->fieldE9;
            *(u16 *)(body + 0x3E8) = 0x1E;
            return;
        }
    }
    if (actor->field2B0) {
        actor->state2A8 = 0;
        actor->phase2A9 = 0;
        actor->behavior2AA = 4;
        actor->counter2AC = 0;
        actor->field2B1 = 1;
    } else {
        actor->counter2AC++;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080844B8);
#endif
