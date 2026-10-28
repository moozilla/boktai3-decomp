#include "global.h"
#ifdef NONMATCHING
// fuzzy: 84.1%
// Remaining differences are flag-pointer lifetimes, PRE spills, turn-loop allocation, and address reloads.
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
#include "player.h"

s32 Callback_InvokeActor(void *, void *);
s32 Nav_IsPositionInMesh(void *, struct ContactPosition *, struct ContactPosition *);
s32 Terrain_ApplyBlocked(void *, struct ContactPosition *, struct ContactPosition *);
void sub_0807E334(struct SceneActorRegistryResetView *);
void Sound_PlaySong(u32);

#define F8(a, o)  (*(u8 *)((u8 *)(a) + (o)))
#define F16(a, o) (*(u16 *)((u8 *)(a) + (o)))
#define S16(a, o) (*(s16 *)((u8 *)(a) + (o)))
#define F32(a, o) (*(u32 *)((u8 *)(a) + (o)))
#define PTR(a, o) (*(void **)((u8 *)(a) + (o)))

struct RecoveryRoutePoint {
    s16 x, z;
};
struct RecoveryRouteLink {
    u8 unknown0[8];
    struct RecoveryRoutePoint *point;
};
struct RecoveryFlags16 {
    u16 value;
};
#define HOME (*(struct ContactPosition **)(group + 0x200))
struct RecoveryTurn {
    u8 direction, savedDirection, mode;
};
struct RecoverySavedControl {
    void *target;
    u8 state, phase, behavior, kind;
};

static inline u8 TakeFlag2B2(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B2) {
        actor->field2B2 = 0;
        return 1;
    }
    return 0;
}
static inline s32 Quarter(s32 v)
{
    return v >= 0 ? v >> 2 : -(-v >> 2);
}
static inline s32 Clamp(s32 v)
{
    if (v > 200)
        return 200;
    if (v < -200)
        return -200;
    return v;
}
static inline u8 AtRest(s32 x, s32 z)
{
    if (!x && !z)
        return 1;
    return 0;
}
static inline u32 Flag16(void *p, u32 maskInput, u32 off)
{
    u32 mask = maskInput;
    return *(u16 *)((u8 *)p + off) & mask;
}
static inline u32 Flag32(void *p, u32 maskInput, u32 off)
{
    u32 mask = maskInput;
    return *(u32 *)((u8 *)p + off) & mask;
}
static inline u8 TestBits16(u16 *p, u32 maskInput)
{
    u32 mask = maskInput;
    if (*p & mask)
        return 1;
    return 0;
}
static inline s32 PlayerSlot(u8 *actor)
{
    if (actor[8] <= 3)
        return actor[8];
    return -1;
}
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline void SetControl(struct SceneActorRegistryResetView *actor, u32 kind, void *control)
{
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
static inline void SetState(struct SceneActorRegistryResetView *actor, u32 state, u32 phase,
                            u32 behavior)

{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline u16 MaskFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2D4 & mask;
}
static inline u32 KeepFlagPointer(struct SceneActorRegistryResetView *actor, u32 mask, u16 **out)
{
    u16 *flags = &actor->flags2D4;
    *out = flags;
    return *flags & mask;
}
static inline u16 *Flags2D4(struct SceneActorRegistryResetView *actor)
{
    return &actor->flags2D4;
}
static inline u32 CellIndex(struct ContactPosition *p)
{
    s32 x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline u32 CellHeight(u32 cell, struct ContactPosition *position)
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
static inline u8 TryMove(struct SceneActorRegistryResetView *actor, void *route,
                         struct ContactPosition *position, struct ContactPosition *destination)
{
    s32 result;
    if (!Nav_IsPositionInMesh(route, position, destination)) {
        u32 mask = ~0x200;
        actor->flags2D4 = mask & actor->flags2D4;
        return 0;
    } else {
        result = Terrain_ApplyBlocked(route, position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear(&actor->motion, 8);
            return 1;
        } else {
            u32 mask = ~0x200;
            actor->flags2D4 = mask & actor->flags2D4;
            return 0;
        }
    }
}
static inline u32 Random(const u16 *table, u32 *seed, u32 wrap, u32 bits)
{
    u32 index = (*seed + 1) & wrap;
    u32 v;
    *seed = index;
    index *= 2;
    v = *(const u16 *)(index + (u32)table);
    v &= bits;
    return v;
}
static inline u32 RandomDirectionExcept(u32 current)
{
    s32 i;
    for (i = 0; i <= 3; i++) {
        u32 d = Random(gUnk_0203B400, &gRandomIndex, 0x3FF, 3) << 6;
        if (d != current)
            return d;
    }
    return current;
}
static inline void Wander(struct SceneActorRegistryResetView *input, u8 direction)
{
    struct SceneActorRegistryResetView *actor = input;
    u8 *group = (u8 *)&actor->group;
    if (!Flag16(actor, 0x400, 0x2D4) && !TestBits16((u16 *)(group + 0x40), 4)) {
        ((struct RecoveryTurn *)(group + 0x204))->direction =
            RandomDirectionExcept(actor->direction);
        ((struct RecoverySavedControl *)(group + 0x8C))->target = actor->target;
        ((struct RecoverySavedControl *)(group + 0x8C))->kind = actor->field2B5;
        ((struct RecoverySavedControl *)(group + 0x8C))->state = actor->state2A8;
        ((struct RecoverySavedControl *)(group + 0x8C))->phase = actor->phase2A9;
        ((struct RecoverySavedControl *)(group + 0x8C))->behavior = actor->behavior2AA;
        group[0x205] = direction;
        {
            void *target = PTR(group, 0x21C);
            SetControl(actor, 0x1B, target);
        }
        SetState(actor, 3, 1, 0);
        group[0x206] = 6;
    }
}
static inline u8 Chase(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor;
    u8 *group;
    u32 cell;
    s32 slot;
    u32 *targetp;
    actor = input;
    group = (u8 *)&actor->group;
    targetp = &actor->fieldDC;
    {
        struct ContactPosition *dest = actor->state5D4;
        struct RecoveryRouteLink *link = (void *)&actor->route;
        dest->x = link->point->x;
        dest->z = link->point->z;
        dest->y = CellHeight(CellIndex(dest), dest);
    }
    if (Test(gEngineFlags, 0x4800)) {
        u8 *target = (u8 *)*targetp;
        if (!target)
            goto fail;
        slot = PlayerSlot(target);
        if (slot < 0)
            goto fail;
        cell = *(u16 *)((u8 *)PTR(gPlayers[slot], 0x40) + 6);
    } else {
        cell = CellIndex((struct ContactPosition *)(*targetp + 0xC));
        slot = 0;
    }
    if (cell == actor->terrainCell || Flag32(gPlayers[slot], 0x20000, 0x20)) {
        u16 busy = Flag16(actor, 0x400, 0x2D4);
        u16 blocked;
        if (busy)
            return 0;
        {
            struct RecoveryFlags16 *flags = (void *)(group + 0x40);
            s32 mask = ~4;
            flags->value = mask & flags->value;
        }
        blocked = Flag16(actor, 0x80, 0x2D4);
        if (blocked) {
            void *target = PTR(group, 0x48);
            SetControl(actor, 0x1A, target);
            SetState(actor, 0, 0, 12);
        } else {
            {
                void *target = actor->entryTarget228;
                SetControl(actor, 1, target);
            }
            SetState(actor, 0, 0, 5);
            {
                struct ContactPosition *dest = *(struct ContactPosition **)(group + 0x200);
                struct RecoveryRouteLink *link = (struct RecoveryRouteLink *)(group + 0x74);
                dest->x = link->point->x;
                dest->z = link->point->z;
                dest->y = CellHeight(CellIndex(dest), dest);
                HOME->y = CellHeight(CellIndex(HOME), HOME);
                HOME->pad = 0;
            }
        }
        Wander(input, actor->direction);
        sub_0807E334(input);
        actor->field2C2 = 0x20;
        return 0;
    }
    {
        struct ContactPosition center;
        SceneActorTerrain_GetCenter(cell, &center);
        if (!TryMove(input, PTR(group, 0x1FC), &actor->position, &center)) {
        fail:
            Callback_InvokeActor(actor, PTR(group, 0x224));
            return 0;
        }
    }
    return 1;
}

void Sound_PlaySong(u32);
void SparkParticle_Spawn(void *, u32, void *, u32, u32, u32, u32, u32);
void Mummy_ResumeRouteAfterRecovery(void);
void SceneActorMovement_ResolveRoutePosition(void *);
struct RecoveryAnimation {
    u8 unknown0[7];
    u8 duration;
    u16 index;
    u8 unknownA[4];
    u16 elapsed;
};
struct RecoveryRoutePoints {
    u8 count, unknown1;
    u16 offset;
};
struct RecoveryRoute {
    u8 unknown0[2];
    u8 index, phase;
    struct RecoveryRoutePoints *points;
    void *current;
};
static inline s32 AxisDelta(struct SceneActorRegistryResetView *actor, u32 offset, s16 **out)
{
    s32 source = *(s16 *)((u8 *)actor + offset);
    s16 *current = (void *)((u8 *)actor + offset + 8);
    s32 value = source - *current;
    *out = current;
    return Quarter(value);
}
static inline u8 Stopped(s32 x, s32 z)
{
    if (!x && !z)
        return 1;
    return 0;
}
static inline u8 RetryTarget(struct SceneActorRegistryResetView *actor, u8 *group)
{
    return TryMove(actor, *(void **)(group + 0x1FC), &actor->position,
                   (struct ContactPosition *)(actor->fieldDC + 12));
}
// confidence=low: recoil easing completes into a route or chase recovery state.
#define Mummy_UpdateDisplacementRecovery sub_080D2584
void Mummy_UpdateDisplacementRecovery(struct SceneActorRegistryResetView *actor, s32 frame)
{
    struct RecoveryAnimation *anim = (void *)((u8 *)actor->drawable + 0x48);
    struct ContactPackedVector range;
    s16 *x, *z;
    s32 dx, dz;
    u8 *group;
    u32 *actorFlags, *flagsPointer;
    if (SceneActorReaction_TakePrepared(actor)) {
        u16 *flags;
        *(s16 *)((u8 *)actor + 0x2EC) = Quarter(*(s16 *)((u8 *)actor + 0x2EC));
        *(s16 *)((u8 *)actor + 0x2F0) = Quarter(*(s16 *)((u8 *)actor + 0x2F0));
        SceneActorAnimation_SelectDirectionalEntry(actor, 3);
        if (KeepFlagPointer(actor, 8, &flags)) {
            Sound_PlaySong(0x12A);
            Sound_PlaySong(0xFD);
        }
        {
            s32 mask = ~1;
            *flags = mask & *flags;
        }
        actor->field2B3 = 0;
    }
    {
        u32 mask = 0x2000;
        flagsPointer = &actor->flags2CC;
        *flagsPointer |= mask;
    }
    {
        u32 mask = 0x10000;
        actor->flags2D0 |= mask;
    }
    if (frame <= 17 && anim->index == 0 && anim->elapsed == (anim->duration != 0) &&
        anim->duration != 1)
        anim->elapsed = 0;
    if (Mod(frame, 12) == 0 && MaskFlags(actor, 8)) {
        u32 index;
        u32 sample;
        range.x = 60;
        range.y = 60;
        range.z = 40;
        index = (gRandomIndex + 1) & 1023;
        gRandomIndex = index;
        index *= 2;
        sample = *(const u16 *)((u32)gUnk_0203B400 + index);
        sample &= 3;
        sample += 1;
        SparkParticle_Spawn(&actor->position, sample, &range, actor->direction + 128, 10, 6, 2, 2);
    }
    actorFlags = flagsPointer;
    dx = AxisDelta(actor, 0x310, &x);
    dz = AxisDelta(actor, 0x314, &z);
    if (dx > 200)
        dx = 200;
    else if (dx < -200)
        dx = -200;
    if (dz > 200)
        dz = 200;
    else if (dz < -200)
        dz = -200;
    actor->motion.x += dx;
    actor->motion.z += dz;
    *x += dx;
    *z += dz;
    if (!Stopped(dx, dz))
        return;
    if (!actor->field2B3)
        return;
    group = (u8 *)&actor->group;
    actor->field2B0 = 1;
    {
        s32 mask = ~0x120;
        actor->flags2D4 = mask & actor->flags2D4;
    }
    {
        s32 mask = ~0x80;
        *actorFlags &= mask;
    }
    if (MaskFlags(actor, 0x10)) {
        struct RecoveryRoute *route = actor->currentRoute;
        u8 choice;
        u32 index = (gRandomIndex + 1) & 1023;
        s32 sample;
        gRandomIndex = index;
        sample = gUnk_0203B400[index];
        choice = Mod(sample >> 3, route->points->count);
        if (choice == route->index)
            choice = Mod(choice + 1, route->points->count);
        route->index = choice;
        route->phase = 0;
        {
            u8 *base = *(u8 **)((u8 *)gTerrainMap + 0x10);
            base += route->points->offset;
            route->current = base + route->index * 8;
        }
        SceneActorMovement_ResolveRoutePosition(actor);
        SetState(actor, 0, 0, 8);
        {
            void *target = *(void **)((u8 *)actor + 0x5E8);
            SetControl(actor, 0x13, target);
        }
        {
            s32 mask = ~8;
            actor->flags2D4 = mask & actor->flags2D4;
        }
        return;
    }
    if (actor->field2DC & 0x80000) {
        {
            u32 mask = 2;
            u16 *flags = &actor->flags2D6;
            *flags = mask | *flags;
        }
        return;
    }
    if (MaskFlags(actor, 8)) {
        SetState(actor, 3, 0, 0);
        {
            void *target = Mummy_ResumeRouteAfterRecovery;
            SetControl(actor, 0x14, target);
        }
        {
            s32 mask = ~8;
            actor->flags2D4 = mask & actor->flags2D4;
        }
        actor->field2C2 = 64;
        return;
    }
    if (MaskFlags(actor, 0x80)) {
        *actorFlags |= 0x80;
        actor->fieldDC = (u32)((u8 *)gPlayers[0] + 0x24);
        if (Chase(actor) || RetryTarget(actor, group)) {
            *(u16 *)(group + 0x40) |= 4;
            {
                void *target = *(void **)(group + 0x48);
                SetControl(actor, 0x1A, target);
            }
            SetState(actor, 0, 0, 12);
            return;
        }
        goto failed;
    } else {
        *actorFlags |= 0x10;
        (*(struct ContactPosition **)(group + 0x200))->pad = 0;
        actor->fieldDC = (u32)((u8 *)gPlayers[0] + 0x24);
        if (!Chase(actor) && !RetryTarget(actor, group)) {
        failed:
            Callback_InvokeActor(actor, *(void **)(group + 0x224));
        } else {
            {
                void *target = actor->entryTarget228;
                SetControl(actor, 1, target);
            }
            SetState(actor, 0, 0, 7);
            actor->field2C2 = Div(64, 3);
        }
    }
    return;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_080D2584);
#endif
