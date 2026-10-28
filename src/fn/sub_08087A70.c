#include "global.h"
#ifdef NONMATCHING
// fuzzy: 78.9%
// Remaining differences are helper control flow, pointer lifetimes, and register allocation.
#include "global.h"
#include "scene_actor_terrain.h"
#include "scene_actor_reaction.h"
#include "engine.h"

void Skeleton_UpdateScriptWaitState(void);
void sub_0807E334(void *);
s32 Mod(s32, s32);
s32 Div(s32, s32);
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);

struct ChasePlayer {
    u8 unknown0[0x20];
    u32 flags;
    u8 unknown24[0x1C];
    u16 *cell;
};
extern struct ChasePlayer *gPlayers[];

struct ChaseTarget {
    u8 unknown0[8];
    u8 index;
    u8 unknown9[3];
    struct ContactPosition position;
};

static inline u8 Chase_Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline void Chase_SetCtl(struct SceneActorRegistryResetView *actor, void *cb, u32 k)
{
    void *callback = cb;
    u32 kind = k;
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = callback;
    actor->field2C8 = 0;
    {
        u32 *field = &actor->flags114;
        u32 mask = ~1;
        *field &= mask;
    }
    actor->word104 = 0;
}
static inline void Chase_SetState(struct SceneActorRegistryResetView *actor, u32 s, u32 p, u32 b)
{
    u32 behavior = b;
    actor->state2A8 = s;
    actor->phase2A9 = p;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline void Chase_SetBehavior(struct SceneActorRegistryResetView *actor, u32 b)
{
    actor->state2A8 = 0;
    actor->phase2A9 = 0;
    actor->behavior2AA = b;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline u16 Chase_MotionFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2D4 & mask;
}
static inline u32 Chase_Bits32(u32 *field, u32 mask)
{
    return *field & mask;
}
static inline s32 Chase_CellIndex(s32 x, s32 z)
{
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline s32 Chase_CellAt(struct ContactPosition *position)
{
    return Chase_CellIndex(position->x >> 8, position->z >> 8);
}
static inline u32 Chase_HeightAt(s32 cell, struct ContactPosition *position)
{
    u8 *data = TerrainModifiers_FindCellOverride(cell, 1);
    u32 kind, height;
    if (data)
        data += 4;
    else
        data = gTerrainMap->cells + (cell * 4 + 12);
    kind = *data >> 4;
    height = (*data & 15) << 8;
    if (kind == 1)
        goto slopeZ;
    if (kind == 2)
        goto slopeX;
    goto done;
slopeZ:
    height -= *(u8 *)&position->z;
    goto done;
slopeX:
    height -= *(u8 *)&position->x;
done:
    return height;
}
static inline u32 Chase_Height(struct ContactPosition *position)
{
    return Chase_HeightAt(Chase_CellAt(position), position);
}
static inline void Chase_SnapToRoute(struct ContactPosition *p, u8 *route)
{
    p->x = (*(u16 **)(route + 8))[0];
    p->z = (*(u16 **)(route + 8))[1];
    p->y = Chase_Height(p);
}
static inline void Chase_ClearBits(u16 *field, u32 bits)
{
    *field &= ~bits;
}
static inline u8 Chase_TryRoute(struct SceneActorRegistryResetView *actor, void *route,
                                struct ContactPosition *position, struct ContactPosition *target)
{
    s32 result = Nav_IsPositionInMesh(route, position, target);
    if (!result) {
        Chase_ClearBits(&actor->flags2D4, 0x200);
        return 0;
    }
    result = Terrain_ApplyBlocked(route, position, target);
    if (result) {
        if (result != 2)
            Memory_Clear(&actor->motion, 8);
        return 1;
    }
    Chase_ClearBits(&actor->flags2D4, 0x200);
    return 0;
}

struct ChaseFlags40 {
    s16 bits : 16;
};

struct ChasePacket {
    void *control;
    u8 state, phase, behavior, kind;
};
static inline void Chase_SavePacket(void *p, struct SceneActorRegistryResetView *actor)
{
    struct ChasePacket *packet = p;
    packet->control = actor->target;
    packet->kind = actor->field2B5;
    packet->state = actor->state2A8;
    packet->phase = actor->phase2A9;
    packet->behavior = actor->behavior2AA;
}
static inline u32 Chase_RandomBits(u32 mask)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return gRandomTable[index] & mask;
}
static inline void Chase_BeginTurn(void *obj, u8 saved)
{
    struct SceneActorRegistryResetView *actor = obj;
    u8 *group = (u8 *)&actor->group;
    if (!Chase_MotionFlags(actor, 0x400) && !Chase_Test(*(u16 *)((u8 *)actor + 0x414), 4)) {
        s32 prior = actor->direction;
        s32 tries = 0;
        s32 choice;
        do {
            choice = Chase_RandomBits(3) << 6;
            if (choice != prior)
                goto chosen;
        } while (++tries <= 3);
        choice = prior;
    chosen:
        group[0x204] = choice;
        Chase_SavePacket(group + 0x8C, actor);
        group[0x205] = saved;
        Chase_SetCtl(actor, *(void **)(group + 0x21C), 0x1B);
        Chase_SetState(actor, 3, 1, 0);
        group[0x206] = 6;
    }
}

static inline u8 Chase_Arrive(void *obj)
{
    struct ContactPosition target;
    struct SceneActorRegistryResetView *actor = obj;
    u8 *group = (u8 *)&actor->group;
    struct ChaseTarget **slot = (struct ChaseTarget **)((u8 *)actor + 0xDC);
    s32 cell;
    s32 player;
    Chase_SnapToRoute(*(struct ContactPosition **)((u8 *)actor + 0x5D4), (u8 *)actor + 0x448);
    if (Chase_Test(gEngineFlags, 0x4800)) {
        struct ChaseTarget *t = *slot;
        if (!t)
            goto invoke;
        {
            u32 index = t->index;
            player = -1;
            if (index <= 3)
                player = index;
        }
        if (player < 0)
            goto invoke;
        cell = gPlayers[player]->cell[3];
    } else {
        struct ChaseTarget *t = *slot;
        cell = Chase_CellIndex(t->position.x >> 8, t->position.z >> 8);
        player = 0;
    }
    if (cell != actor->terrainCell && !Chase_Bits32(&gPlayers[player]->flags, 0x20000))
        goto reroute;
    if (Chase_MotionFlags(actor, 0x400))
        goto done;
    ((struct ChaseFlags40 *)(group + 0x40))->bits &= ~4;
    if (Chase_MotionFlags(actor, 0x80)) {
        Chase_SetCtl(actor, *(void **)(group + 0x48), 0x1A);
        Chase_SetBehavior(actor, 12);
    } else {
        Chase_SetCtl(actor, actor->entryTarget228, 1);
        Chase_SetBehavior(actor, 5);
        Chase_SnapToRoute(*(struct ContactPosition **)(group + 0x200), group + 0x74);
        (*(struct ContactPosition **)(group + 0x200))->y =
            Chase_HeightAt(Chase_CellAt(*(struct ContactPosition **)(group + 0x200)),
                           *(struct ContactPosition **)(group + 0x200));
        (*(struct ContactPosition **)(group + 0x200))->pad = 0;
    }
    {
        u8 saved = actor->direction;
        Chase_BeginTurn(obj, saved);
    }
    sub_0807E334(actor);
    actor->field2C2 = 0x20;
    goto done;
reroute: {
    struct ContactPosition *targetPointer = &target;
    s32 x = Mod(cell, *(s16 *)(gTerrainMap->cells + 4));
    s32 z = Div(cell, *(s16 *)(gTerrainMap->cells + 4));
    u8 *data;
    targetPointer->x = (x << 8) + 0x80;
    data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else
        data = gTerrainMap->cells + (cell * 4 + 12);
    targetPointer->y = (*data & 15) << 8;
    targetPointer->z = (z << 8) + 0x80;
    if (Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->position, targetPointer))
        return 1;
invoke:
    Callback_InvokeActor(actor, *(void **)(group + 0x224));
}
done:
    return 0;
}

void Sound_PlaySong(u32);
void SparkParticle_Spawn(void *, u32, void *, u32, u32, u32, u32, u32);
void sub_08087018(void);
struct RecoveryAnimation {
    u8 unknown0[7];
    u8 duration;
    u16 index;
    u8 unknownA[4];
    u16 elapsed;
};
static inline s32 Quarter(s32 value)
{
    if (value >= 0)
        return value >> 2;
    return -((-value) >> 2);
}
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
    return Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->position,
                          &((struct ChaseTarget *)actor->fieldDC)->position);
}
// confidence=low: recoil easing completes into a route or chase recovery state.
#define Bat_UpdateKnockbackRecovery sub_08087A70
void Bat_UpdateKnockbackRecovery(struct SceneActorRegistryResetView *actor, s32 frame)
{
    struct RecoveryAnimation *anim = (void *)((u8 *)actor->drawable + 0x48);
    struct ContactPackedVector range;
    s16 *x, *z;
    s32 dx, dz;
    u8 *group = (u8 *)&actor->group;
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry(actor, 3);
        if (actor->flags2D4 & 8) {
            Sound_PlaySong(0xD4);
            Sound_PlaySong(0x12A);
        }
        {
            s32 mask = ~1;
            actor->flags2D4 = mask & actor->flags2D4;
        }
        actor->field2B3 = 0;
    }
    {
        u32 mask = 0x2000;
        actor->flags2CC |= mask;
    }
    {
        u32 mask = 0x10000;
        actor->flags2D0 |= mask;
    }
    if (frame <= 17 && anim->index == 0 && anim->elapsed == (anim->duration != 0) &&
        anim->duration != 1)
        anim->elapsed = 0;
    if (Mod(frame, 12) == 0 && Chase_MotionFlags(actor, 8)) {
        u32 index;
        u32 sample;
        range.x = 60;
        range.y = 60;
        range.z = 40;
        index = (gRandomIndex + 1) & 1023;
        gRandomIndex = index;
        index *= 2;
        sample = *(const u16 *)((u32)gRandomTable + index);
        sample &= 3;
        SparkParticle_Spawn(&actor->position, sample + 1, &range, actor->direction + 128, 10, 6, 2,
                            2);
    }
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
    actor->field2B0 = 1;
    {
        s32 mask = ~0x120;
        actor->flags2D4 = mask & actor->flags2D4;
    }
    {
        s32 mask = ~0x80;
        actor->flags2CC &= mask;
    }
    if (actor->field2DC & 0x80000) {
        actor->flags2D6 |= 2;
        return;
    }
    if (Chase_MotionFlags(actor, 8)) {
        Chase_SetState(actor, 3, 0, 0);
        Chase_SetCtl(actor, sub_08087018, 0x14);
        {
            s32 mask = ~8;
            actor->flags2D4 = mask & actor->flags2D4;
        }
        actor->field2C2 = 64;
        return;
    }
    if (actor->flags2D4 & 0x80) {
        actor->flags2CC |= 0x80;
        *(u16 *)(group + 0x40) |= 4;
        actor->fieldDC = (u32)((u8 *)gPlayers[0] + 0x24);
        if (Chase_Arrive(actor) || RetryTarget(actor, group)) {
            Chase_SetCtl(actor, *(void **)(group + 0x48), 0x1A);
            Chase_SetBehavior(actor, 12);
            return;
        }
        goto failed;
    } else {
        actor->fieldDC = (u32)((u8 *)gPlayers[0] + 0x24);
        actor->flags2CC |= 0x10;
        (*(struct ContactPosition **)(group + 0x200))->pad = 0;
        if (Chase_Arrive(actor) || RetryTarget(actor, group)) {
            Chase_SetCtl(actor, actor->entryTarget228, 1);
            Chase_SetBehavior(actor, 7);
            actor->field2C2 = 32;
            return;
        }
    }
failed:
    Callback_InvokeActor(actor, *(void **)(group + 0x224));
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08087A70);
#endif
