#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
#ifdef NONMATCHING
// fuzzy: 93.0% (agent draft, 18 structural / 69 total insns off; see header notes below)
/* Skeleton: approach the target cell (tentative). When actor[0x6C] bit 0 is set: route
 * to the partner's (co-op: player slot's) cell; on arrival either resume group+0x48 (kind
 * 0x1A, flag 0x80) or restart entry 0x228 (kind 1) and pick a new random facing (Turn, kind
 * 0x1B). Otherwise steer toward the partner with min/max distance bands and drive the walk
 * animation from the callback at +0x260.
 * Draft status: 18 structural / 69 total differing insns (anchordiff). Remaining:
 *  - the QI zero for 2B0/2A8/2A9 in the arrival branches: ROM materialises it in that block
 *    (movs; mov r8); here `zero` is set before the co-op test, so it overlaps partnerp
 *    (r8 vs sl) and Turn's group copy is spilled instead of living in sl.
 *  - Turn's hoisted address slots come out in a different order: GCSE PRE reaching regs are
 *    numbered in hash order ((13772+R+k) % T); no T works with Turn's actor/group copies
 *    adjacent (R, R+1), so the original's Turn arguments were created elsewhere. */
void Skeleton_UpdateScriptWaitState(void);
void sub_0807E334(void *);
s32 Callback_InvokeActor(void *, void *);
s32 Math_Atan2(s32, s32);
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);
extern u8 *gPlayers[4];

struct SkeletonChaseBody {
    u8 unknown0[0x6DC];
    u32 flags;
    u8 unknown6E0[0x28];
    u32 minimumDistance, maximumDistance;
};
struct SkeletonRoutePoint {
    s16 x, z;
};
struct SkeletonRouteLink {
    u8 unknown0[8];
    struct SkeletonRoutePoint *point;
};

static inline u8 TakeFlag2B1(void *a)
{
    struct SceneActorRegistryResetView *actor = a;
    if (actor->field2B1) {
        actor->field2B1 = 0;
        return 1;
    }
    return 0;
}
static inline void SetControl(void *a, void *control, u32 kind)
{
    struct SceneActorRegistryResetView *actor = a;
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
static inline void SetControlZ(void *a, void *control, u32 kind, u8 zero)
{
    struct SceneActorRegistryResetView *actor = a;
    actor->field2B2 = 1;
    actor->field2B0 = zero;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
static inline void SetState(void *a, u8 s, u8 p, u32 b)
{
    struct SceneActorRegistryResetView *actor = a;
    actor->state2A8 = s;
    actor->phase2A9 = p;
    actor->behavior2AA = b;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline s32 PlayerSlot(u8 *actor)
{
    if (actor[8] <= 3)
        return actor[8];
    return -1;
}
static inline u32 CellIndex(struct ContactPosition *p)
{
    s32 x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline u32 CellHeight(struct ContactPosition *position, u32 cell)
{
    u32 shape, height;
    u8 *data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = cell * 4 + 12;
        data = map->cells + offset;
    }
    shape = *data >> 4;
    height = (*data & 15) << 8;
    if (shape == 1)
        goto slopeZ;
    if (shape == 2)
        goto slopeX;
    goto done;
slopeZ:
    height -= *((u8 *)position + 4);
    goto done;
slopeX:
    height -= *(u8 *)position;
done:
    return height;
}
static inline void ClearFlags16(u16 *p, u32 mask)
{
    *p &= mask;
}
static inline u8 TryMove2(struct SceneActorRegistryResetView *actor, void *route,
                          struct ContactPosition *position, struct ContactPosition *destination)
{
    s32 result;
    if (!Nav_IsPositionInMesh(route, position, destination)) {
        ClearFlags16(&actor->flags2D4, ~0x200);
        return 0;
    } else {
        result = Terrain_ApplyBlocked(route, position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear(&actor->motion, 8);
            return 1;
        } else {
            ClearFlags16(&actor->flags2D4, ~0x200);
            return 0;
        }
    }
}
static inline u16 Masked(void *a, u32 m)
{
    struct SceneActorRegistryResetView *actor = a;
    u32 mask = m;
    return actor->flags2D4 & mask;
}
static inline s32 Scale(s32 n)
{
    return n >= 0 ? n >> 16 : -((-n) >> 16);
}

static inline u8 Test16(u16 *p, u32 mask)
{
    if (*p & mask)
        return 1;
    return 0;
}
static inline u32 PickTurn(u32 dir)
{
    u32 d;
    s32 i;
    for (i = 0; i < 4; i++) {
        {
            u32 index = (gRandomIndex + 1) & 1023;
            u32 v;
            gRandomIndex = index;
            index *= 2;
            v = *(const u16 *)(index + (u32)gUnk_0203B400);
            d = (v & 3) << 6;
        }
        if (d != dir)
            return d;
    }
    return dir;
}
struct SkeletonSavedControl {
    void *target;
    u8 state, phase, behavior, kind;
};
static inline void Turn(u8 saved, void *ctx, void *g)
{
    struct SceneActorRegistryResetView *actor = ctx;
    u8 *group = g;
    {
        u32 mask = 0x400;
        if (((struct SceneActorRegistryResetView *)ctx)->flags2D4 & mask)
            return;
    }
    if (Test16((u16 *)((u8 *)ctx + 0x414), 4))
        return;
    group[0x204] = PickTurn(actor->direction);
    {
        struct SkeletonSavedControl *save = (struct SkeletonSavedControl *)(group + 0x8C);
        save->target = actor->target;
        save->kind = actor->field2B5;
        save->state = actor->state2A8;
        save->phase = actor->phase2A9;
        save->behavior = actor->behavior2AA;
    }
    group[0x205] = saved;
    SetControl(actor, *(void **)(group + 0x21C), 0x1B);
    SetState(actor, 3, 1, 0);
    group[0x206] = 6;
}
static inline void ResetDestination(void *ctx)
{
    u8 *group = ctx;
    struct ContactPosition *dest = *(struct ContactPosition **)(group + 0x200);
    struct SkeletonRouteLink *link = (struct SkeletonRouteLink *)(group + 0x74);
    dest->x = link->point->x;
    dest->z = link->point->z;
    dest->y = CellHeight(dest, CellIndex(dest));
}
#define DEST (*(struct ContactPosition **)(group + 0x200))
#define CTX  ((struct SceneActorRegistryResetView *)ctx)
static inline void Approach(void *ctx)
{
    struct SceneActorRegistryResetView *actor = ctx;
    u8 *group = (u8 *)&CTX->group;
    u8 **partnerp = (u8 **)&CTX->fieldDC;
    u32 cell;
    s32 index;
    u8 zero;
    ResetDestination(group);
    zero = 0;
    if (Test(gEngineFlags, 0x4800)) {
        u8 *partner = *partnerp;
        if (!partner || (index = PlayerSlot(partner)) < 0) {
            Callback_InvokeActor(actor, *(void **)(group + 0x224));
            return;
        }
        cell = *(u16 *)(*(u8 **)(gPlayers[index] + 0x40) + 6);
    } else {
        cell = CellIndex((struct ContactPosition *)(*partnerp + 0xC));
        index = 0;
    }
    if (cell != actor->terrainCell) {
        u8 *player = gPlayers[index];
        u32 mask = 0x20000;
        if (!(*(u32 *)(player + 0x20) & mask))
            goto move;
    }
    {
        {
            u32 mask = 0x400;
            u32 v = (u16)(actor->flags2D4 & mask);
            if (v)
                return;
        }
        {
            u16 *flags = (u16 *)(group + 0x40);
            s32 mask = ~4;
            *flags &= mask;
        }
        if (Masked(actor, 0x80)) {
            SetControlZ(actor, *(void **)(group + 0x48), 0x1A, zero);
            SetState(actor, zero, zero, 12);
        } else {
            SetControlZ(actor, actor->entryTarget228, 1, zero);
            SetState(actor, zero, zero, 5);
            ResetDestination(group);
            {
                u32 c = CellIndex(DEST);
                DEST->y = CellHeight(DEST, c);
            }
            DEST->pad = 0;
        }
        Turn(actor->direction, ctx, (u8 *)&CTX->group);
        sub_0807E334(ctx);
        actor->field2C2 = 0x20;
        return;
    }
move: {
    struct ContactPosition center;
    SceneActorTerrain_GetCenter(cell, &center);
    if (!TryMove2(ctx, *(void **)(group + 0x1FC), &actor->position, &center))
        Callback_InvokeActor(actor, *(void **)(group + 0x224));
}
}

void sub_080C09F0(struct SceneActorRegistryResetView *actor)
{
    struct SkeletonChaseBody *body = actor->context;
    u8 *group = (u8 *)&actor->group;
    if (TakeFlag2B1(actor)) {
        void *target = Skeleton_UpdateScriptWaitState;
        SetControl(actor, target, 0x26);
    }
    if (**(u8 **)((u8 *)actor + 0x6C) & 1) {
        SetState(actor, 0, 0, 12);
        SetControl(actor, *(void **)(group + 0x48), 0x1A);
        Approach(actor);
        {
            struct SkeletonChaseBody *b = actor->context;
            u32 mask = 0x80;
            b->flags |= mask;
        }
    } else {
        struct ContactPosition diff;
        u32 angle;
        s32 speed;
        {
            struct ContactPosition *out = &diff;
            struct ContactPosition *from = &actor->position;
            struct ContactPosition *to =
                (struct ContactPosition *)(*(u8 **)((u8 *)actor + 0xDC) + 0xC);
            SceneActorTerrain_Subtract(out, to, from);
        }
        angle = Math_Atan2(diff.x, diff.z);
        if (actor->distanceF0 < body->minimumDistance) {
            angle = (angle + 0x80) & 0xFF;
            actor->field2C2 = 0x20;
        } else if (actor->distanceF0 > body->maximumDistance) {
            actor->field2C2 = 0x20;
            actor->direction = angle;
        }
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
        speed = Callback_InvokeActor(actor, *(void **)((u8 *)actor + 0x260));
        {
            u32 mask = 0xA02F;
            u16 *scale;
            if (actor->flags2D4 & mask) {
                actor->field2C2 = 0x40;
                scale = &actor->field2C2;
            } else {
                u16 t = actor->field2C2;
                scale = &actor->field2C2;
                if (t == 0)
                    *scale = 0x40;
            }
            if (actor->drawableKind == 0)
                Anim_SetDurationScale((u8 *)actor->drawable + 0x48, *scale);
            else
                Anim_SetFrameDurationScale((u8 *)actor->drawable + 0x20, *scale);
        }
        actor->motion.x += Scale(gUnk_086149C4[(angle + 0x40) & 0xFF] * speed);
        actor->motion.z += Scale(gUnk_086149C4[angle & 0xFF] * speed);
        {
            u32 mask = 0x2000;
            actor->flags2CC |= mask;
        }
        if (actor->distanceF0 > body->minimumDistance &&
            actor->distanceF0 < body->maximumDistance) {
            u16 mask = 0x80;
            actor->flags2D6 = mask | actor->flags2D6;
        } else
            actor->counter2AC++;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080C09F0);
#endif
