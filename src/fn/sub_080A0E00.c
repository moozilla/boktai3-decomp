#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
extern const s16 gUnk_086149C4[];
extern u8 *gUnk_02000580[];
s32 Math_Atan2(s32, s32);
s32 Div(s32, s32);
void CpuSet(const void *, void *, u32);
void sub_0809345C(void *, void *);
void Bee_SubVector(void *, void *, void *);
s32 Bee_GetMovementVariant(void *);
#define Crow_UpdateDiveAttack sub_080A0E00
#ifdef NONMATCHING
// fuzzy: 98.0%
// Remaining differences: instruction ordering and temporary registers.
static inline u8 Take(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
static inline u8 Test(u32 *p, u32 m)
{
    if (*p & m)
        return 1;
    return 0;
}
static inline s32 Shift(s32 n, u32 bits)
{
    if (n >= 0)
        return n >> bits;
    return -((-n) >> bits);
}
static inline u32 Rand(void)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return gUnk_0203B400[gUnk_03005308];
}
static inline void Polar(struct ContactPosition *out, u32 angle, s32 speed)
{
    out->x = Shift(gUnk_086149C4[(angle + 64) & 255] * speed, 12);
    out->y = 0;
    out->z = Shift(gUnk_086149C4[angle & 255] * speed, 12);
}
static inline void Delta(struct ContactPosition *out, const struct ContactPosition *from,
                         const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}
static inline void Add(struct ContactPosition *out, const struct ContactPosition *from)
{
    out->x = from->x + out->x;
    out->y = out->y + from->y;
    out->z = from->z + out->z;
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
static inline void State(struct SceneActorRegistryResetView *a)
{
    u32 behavior = 2;
    a->state2A8 = 0;
    a->phase2A9 = 2;
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

void Crow_UpdateDirectionalPose(void);
void Sound_PlaySong(u32);
s32 Callback_InvokeActor(void *, void *);
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);
void TentacleActor_SelectWanderRoute(void *);
void TentacleActor_AdvanceMotionPhase(void *, s32);
void sub_080A9DB0(void *, s32, s32);
void TentacleActor_UpdateOscillation(void *, s32, s32);
static inline u8 Approach(struct SceneActorRegistryResetView *actor, struct SavedMotion *saved,
                          u8 *speedp)
{
    u8 *motion = (u8 *)actor + 0x50;
    s32 speed = *speedp;
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

// MATCHING_CONCESSION: register-pinning

void Crow_UpdateDiveAttack(struct SceneActorRegistryResetView *actor)
{
    u8 *body = actor->context;
    struct ContactPosition delta;
    if (Take(&actor->field2B1)) {
        s32 x, z;
        Control(actor, Crow_UpdateDirectionalPose, 0x25);
        *(struct SavedPair *)(body + 0x3fc) = *(struct SavedPair *)(gUnk_02000580[0] + 0x30);
        { register void *dest asm("r1") = body + 0x42c; u32 zero; asm("" : "+r"(dest)); zero = 0; CpuSet(&zero, dest, 0x05000002); }
        Delta(&delta, &actor->position, (void *)(body + 0x3fc));
        *((u8 *)actor + 0x59) = Math_Atan2(delta.x, delta.z);
        *(u16 *)(body + 0x42c) = Shift(gUnk_086149C4[(*((u8 *)actor + 0x59) + 64) & 255] * body[0x3d9], 12);
        { register s32 value asm("r1"); u32 zero = 0; value = Shift(gUnk_086149C4[*((u8 *)actor + 0x59)] * body[0x3d9], 12);
        asm("" : "+r"(value));
        *(u16 *)(body + 0x430) = value;
        *(u16 *)(body + 0x42e) = zero; }
        if (*(s16 *)(body + 0x42c))
            *(s16 *)(body + 0x42c) = Div(delta.x, *(s16 *)(body + 0x42c));
        if (*(s16 *)(body + 0x430))
            *(s16 *)(body + 0x430) = Div(delta.z, *(s16 *)(body + 0x430));
        x = *(s16 *)(body + 0x42c);
        z = *(s16 *)(body + 0x430);
        if (x > z) {
            asm("" ::: "memory");
            if (x) *(s16 *)(body + 0x42e) = Div(delta.y - 32, *(s16 *)(body + 0x42c));
        } else {
            asm("" ::: "memory");
            if (z) *(s16 *)(body + 0x42e) = Div(delta.y - 32, *(s16 *)(body + 0x430));
        }
        Sound_PlaySong(0xf7);
        body[0x3ec] = 0;
        return;
    }
    Callback_InvokeActor(actor, *(void **)((u8 *)actor + 0x270));
    Scale(actor);
    actor->field2EA += *(u16 *)(body + 0x42e);
    if (Mod(actor->counter2AC, 6) == 0) {
        s32 v = *(s16 *)(body + 0x42e);
        *(s16 *)(body + 0x42e) = Shift(v, 1);
    }
    if (Approach(actor, (void *)(body + 0x3f0), body + 0x3d9) || Test((u32 *)(body + 0x3d4), 2)) {
        Polar(&actor->secondaryMotion, *((u8 *)actor + 0x59), body[0x3d9] * 2);
        State(actor);
        return;
    }
    actor->counter2AC++;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_080A0E00);
#endif
