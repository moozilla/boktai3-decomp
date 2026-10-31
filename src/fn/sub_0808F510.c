#include "global.h"
#include "scene_actor_reset.h"
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
s32 Callback_InvokeActor(void *, void *);
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);
void Slime_SelectWanderTarget(void *, u32);
struct SavedMotion {
    u8 pad[12];
    struct ContactPosition position;
};
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
static inline u8 Approach(struct SceneActorRegistryResetView *actor, struct SavedMotion *saved,
                          s32 speed)
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
static inline void Scale(struct SceneActorRegistryResetView *actor)
{
    u16 *scale;
    {
        u32 mask = 0xA02F;
        if (actor->flags2D4 & mask) {
            u16 *p = &actor->field2C2;
            *p = 64;
            scale = p;
        } else {
            u16 *q = &actor->field2C2;
            u16 v = *q;
            scale = q;
            if (v == 0)
                *scale = 64;
        }
    }
    if (actor->drawableKind == 0)
        Anim_SetDurationScale((u8 *)actor->drawable + 0x48, *scale);
    else
        Anim_SetFrameDurationScale((u8 *)actor->drawable + 0x20, *scale);
}
// confidence=low: moves toward a saved waypoint, then re-selects the route.
void Slime_ApproachSavedTarget(struct SceneActorRegistryResetView *actor)
{
    u8 *body = actor->context;
    s32 speed;
    if (Take(&actor->field2B1)) {
        Control(actor, *(void **)((u8 *)actor + 0x228), 1);
        Slime_SelectWanderTarget(actor, *((u8 *)actor + 0x59));
        actor->field2C2 = 0x40;
        return;
    }
    Callback_InvokeActor(actor, *(void **)((u8 *)actor + 0x270));
    speed = Callback_InvokeActor(actor, *(void **)((u8 *)actor + 0x260));
    Scale(actor);
    if (Approach(actor, (void *)(body + 0x3E0), speed))
        Slime_SelectWanderTarget(actor, *((u8 *)actor + 0x59));
    {
        u32 m = 0x2000;
        u32 f = actor->flags2CC;
        f |= m;
        actor->flags2CC = f;
    }
    actor->counter2AC++;
}
