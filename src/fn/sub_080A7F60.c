#include "global.h"
// MATCHING_CONCESSION: register-pinning

#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"

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
    s32 r;
    if (n >= 0)
        r = n >> bits;
    else
        r = -((-n) >> bits);
    return r;
}

static inline void Delta(struct ContactPosition *out, const struct ContactPosition *from,
                         const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
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
static inline void State6(struct SceneActorRegistryResetView *a)
{
    u32 behavior = 6;
    a->state2A8 = 0;
    a->phase2A9 = 0;
    a->behavior2AA = behavior;
    a->counter2AC = 0;
    a->field2B1 = 1;
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

s32 Mod(s32, s32);
static inline u8 Turned(s32 angle)
{
    u8 r = 0;
    if (angle <= 31)
        r = 1;
    return r;
}
// confidence=low: spirals with decreasing speed and restores the saved facing direction.
void TentacleBody_UpdateSpiral(struct SceneActorRegistryResetView *actor)
{
    u32 zero;
    s32 speed;
    s32 amplitude, turn, rate;
    s32 dx, dz;
    if (Take(&actor->field2B1)) {
        Control(actor, *(void **)((u8 *)actor + 0x228), 1);
        {
            u32 m = 0x100000;
            *(u32 *)((u8 *)actor + 0x2D0) |= m;
        }
    }
    if (Mod(actor->counter2AC, 48) == 0)
        Sound_PlaySong(0x3D7);
    {
        u8 *dst = (u8 *)actor + 0x2EC;
        zero = 0;
        {
            u32 *zp = &zero;
            asm("" : "+r"(zp));
            CpuSet(zp, dst, 0x05000002);
        }
    }
    if ((s32)actor->counter2AC > 115) {
        s32 angle = *((u8 *)actor + 0xE9) - actor->body.direction;
        if (angle > 128)
            angle -= 256;
        else if (angle < -128)
            angle += 256;
        if (Turned(angle)) {
            actor->body.direction = *((u8 *)actor + 0xE9);
            State6(actor);
            return;
        }
    }
    if ((s32)actor->counter2AC <= 63) {
        speed = 20;
        amplitude = 8;
        turn = 4;
        rate = 0x1860;
    } else if ((s32)actor->counter2AC <= 115) {
        speed = 14;
        amplitude = 8;
        turn = 8;
        rate = 0x4D8;
    } else {
        speed = 3;
        amplitude = 8;
        turn = 6;
        rate = 0x4D8;
    }
    dx = Shift(gUnk_086149C4[(actor->body.direction + 64) & 255] * speed, 12);
    dz = Shift(gUnk_086149C4[actor->body.direction] * speed, 12);
    actor->body.direction += turn;
    *(u16 *)((u8 *)actor + 0x64) += dx;
    *(u16 *)((u8 *)actor + 0x68) += dz;
    sub_080A9DB0(actor, amplitude, rate);
    actor->counter2AC++;
}
