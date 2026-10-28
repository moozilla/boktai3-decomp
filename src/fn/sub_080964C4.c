#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
#include "scene_actor_route.h"
#include "engine.h"
#include "player.h"
extern const u16 gUnk_0203B400[];
extern const s16 gUnk_086149C4[];
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
static inline s32 Shift(s32 n, u32 bits)
{
    if (n >= 0)
        return n >> bits;
    return -((-n) >> bits);
}
static inline void Polar(struct ContactPosition *out, u32 angle, s32 speed)
{
    out->x = Shift(gUnk_086149C4[(angle + 64) & 255] * speed, 12);
    out->y = 0;
    out->z = Shift(gUnk_086149C4[angle & 255] * speed, 12);
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

void Bee_UpdateSway(void *);
u8 Ghost_SelectRandomRouteEntry(struct SceneActorRouteCursor *);
void Ghost_StartMovementToPosition(void *, void *);
u8 sub_08097798(void *);
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
// confidence=low: follows saved route points and renews a destination after arrival.
void Ghost_FollowRoute(struct SceneActorRegistryResetView *actor)
{
    u8 *body = actor->context;
    s32 speed;
    u32 zero[1];
    struct ContactPosition target;
    if (Take(&actor->field2B1)) {
        Control(actor, actor->entryTarget228, 1);
        Fill(body + 0x5FC, zero, 0);
        body[0x5B7] = 1;
        actor->field2C2 = 64;
    }
    speed = Callback_InvokeActor(actor, *(void **)((u8 *)actor + 0x260));
    Scale(actor);
    Bee_UpdateSway(actor);
    if (Approach(actor, (void *)(body + 0x5C8), speed) && sub_08097798(actor)) {
        struct SceneActorRouteCursor *route;
        struct ContactPosition *p;
        struct ContactPosition *saved;
        s32 cell, height;
        u32 kind;
        u16 attr;
        Polar((void *)(body + 0x5FC), actor->direction, speed);
        route = (void *)(body + 0x60C);
        if (!Ghost_SelectRandomRouteEntry(route))
            goto done;
        p = &target;
        p->x = route->point->x;
        p->z = route->point->z;
        cell = CellIndex(p, &saved);
        attr = *SceneActorTerrain_GetCell(cell);
        kind = attr >> 4;
        {
            u32 mask = 15;
            mask &= attr;
            height = mask << 8;
        }
        switch (kind) {
            case 1:
                height -= ((u8 *)p)[4];
                break;
            case 2:
                height -= ((u8 *)p)[0];
                break;
        }
        p->y = height;
        Ghost_StartMovementToPosition(actor, saved);
    }
done:
    actor->counter2AC++;
}
