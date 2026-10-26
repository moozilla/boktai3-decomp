#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
#include "scene_actor_route.h"
#include "engine.h"
// Zero-store scheduling and the approach speed load still occur in a different order.
extern const u16 gUnk_0203B400[];
extern const s16 gUnk_086149C4[];
extern u8 *gUnk_02000580[];
s32 Math_Atan2(s32, s32);
s32 Div(s32, s32);
void CpuSet(const void *, void *, u32);
void sub_0809345C(void *, void *);
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
static inline s32 Shift(s32 n, u32 bits)
{
    if (n >= 0)
        return n >> bits;
    return -((-n) >> bits);
}
static inline u32 Rand(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return gUnk_0203B400[gRandomIndex];
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
    u32 behavior = 1;
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

void Lun_UpdateSway(void *);
void Lun_SelectRandomRouteEntry(struct SceneActorRouteCursor *);
void Lun_BeginMoveToPosition(void *, void *);
u8 sub_0809CCF4(void *);
static inline void Fill(void *dst, u32 *tmp, u32 value)
{
    *tmp = value;
    CpuSet(tmp, dst, 0x05000002);
}
static inline s32 CellIndex(const struct ContactPosition *position, struct ContactPosition **saved)
{
    s32 x = position->x >> 8;
    s32 z = position->z >> 8;
    *saved = (void *)position;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
void Crow_UpdateDirectionalPose(void *);
void Sound_PlaySong(u32);
static inline void SetState(struct SceneActorRegistryResetView *a, u32 state, u32 phase,
                            u32 behavior)
{
    a->state2A8 = state;
    a->phase2A9 = phase;
    a->behavior2AA = behavior;
    a->counter2AC = 0;
    a->field2B1 = 1;
}
static inline void SetDepthVelocity(u8 *body, s32 value)
{
    u32 zero = 0;
    *(s16 *)(body + 0x430) = value;
    *(s16 *)(body + 0x42e) = zero;
}
static inline u8 BodyFlag(u8 *body, u32 mask)
{
    return Test(*(u32 *)(body + 0x3d4), mask);
}
// confidence=low: advances a directed response, then recoils when it reaches the player or terrain.
static inline u8 Approach_1(struct SceneActorRegistryResetView *actor, struct SavedMotion *saved,
                            u8 *motion, s32 speed)
{
    s16 dx = saved->position.x - (*((u16 *)(motion + 12)));
    s16 dz = saved->position.z - (*((u16 *)(motion + 16)));
    if ((((dx < 0) ? (-dx) : (dx)) <= 32) && (((dz < 0) ? (-dz) : (dz)) <= 32))
        return 1;
    if (dx || dz) {
        motion[9] = Math_Atan2(dx, dz);
        *((u16 *)(motion + 20)) += Shift(gUnk_086149C4[(motion[9] + 64) & 255] * speed, 12);
        *((u16 *)(motion + 24)) += Shift(gUnk_086149C4[motion[9]] * speed, 12);
    }
    return 0;
}

void Crow_ApproachPlayerThenRecoil(struct SceneActorRegistryResetView *actor)
{
    u8 *body = actor->context;
    u32 zero[1];
    struct ContactPosition delta;
    if (Take(&actor->field2B1)) {
        struct ContactPosition *saved;
        Control(actor, Crow_UpdateDirectionalPose, 0x25);
        saved = (void *)(body + 0x3fc);
        *((struct SavedPair *)saved) = *((struct SavedPair *)((*gUnk_02000580) + 0x30));
        Fill(body + 0x42c, zero, 0);
        Delta(&delta, &actor->position, saved);
        actor->direction = Math_Atan2(delta.x, delta.z);
        *((s16 *)(body + 0x42c)) =
            Shift(gUnk_086149C4[(actor->direction + 64) & 255] * body[0x3d9], 12);
        {
            s32 depth = Shift(gUnk_086149C4[actor->direction] * body[0x3d9], 12);
            {
                typeof(*((s16 *)(body + 0x430))) *q1 = &(*((s16 *)(body + 0x430)));
                u32 z1 = 0;
                *q1 = depth;
                *((s16 *)(body + 0x42e)) = z1;
            }
        }
        if (*((s16 *)(body + 0x42c)))
            *((s16 *)(body + 0x42c)) = Div(delta.x, *((s16 *)(body + 0x42c)));
        if (*((s16 *)(body + 0x430)))
            *((s16 *)(body + 0x430)) = Div(delta.z, *((s16 *)(body + 0x430)));
        if ((*((s16 *)(body + 0x42c))) > (*((s16 *)(body + 0x430)))) {
            if (*((s16 *)(body + 0x42c)))
                *((s16 *)(body + 0x42e)) = Div(delta.y - 32, *((s16 *)(body + 0x42c)));
        } else if (*((s16 *)(body + 0x430)))
            *((s16 *)(body + 0x42e)) = Div(delta.y - 32, *((s16 *)(body + 0x430)));
        Sound_PlaySong(247);
        body[0x3ec] = 0;
    } else {
        Callback_InvokeActor(actor, actor->update270);
        Scale(actor);
        *((s16 *)(((u8 *)actor) + 0x2ea)) += *((s16 *)(body + 0x42e));
        if (!Mod(actor->counter2AC, 6))
            *((s16 *)(body + 0x42e)) = Shift(*((s16 *)(body + 0x42e)), 1);
        if (Approach_1(actor, (void *)(body + 0x3f0), ((u8 *)actor) + 0x50, body[0x3d9]) ||
            BodyFlag(body, 2)) {
            Polar(&actor->secondaryMotion, actor->direction, body[0x3d9] * 2);
            SetState(actor, 0, 2, 2);
        } else {
            actor->counter2AC++;
        }
    }
}
