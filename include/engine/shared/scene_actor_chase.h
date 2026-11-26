#ifndef GUARD_SCENE_ACTOR_CHASE_H
#define GUARD_SCENE_ACTOR_CHASE_H
/* Chase-the-player step shared by the 080BBDD0..080C12FC behaviour states (inlined about nine
 * times). Helpers taken from the Skeleton_UpdateTargetApproach draft (81%); Chase_Arrive here returns 1 when
 * the reroute step moved, as 080BBDD0 tests it. Only NONMATCHING drafts include this. */
#include "engine/shared/global.h"
#include "engine/shared/scene_actor_terrain.h"
#include "actors/scene_actor_reaction/scene_actor_reaction.h"
#include "engine/shared/engine_globals.h"
#include "engine/shared/player.h"

void Skeleton_UpdateScriptWaitState(u8 *p);
void SceneActorTerrain_RequestRetreatRoute(struct ChaseActor *);
s32 Mod(s32, s32);
s32 Div(s32, s32);
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);

struct ChaseTarget {
    u8 unknown0[8];
    u8 index;
    u8 unknown9[3];
    struct ContactPosition position;
};

struct ChaseContext {
    u8 unknown0[0x6DC];
    u32 flags;
    u8 unknown6E0[0x28];
    u32 nearDistance;
    u32 farDistance;
};

static inline u8 Chase_TakeArrived(struct ChaseActor *actor)
{
    if (actor->base.field2B1) {
        actor->base.field2B1 = 0;
        return 1;
    }
    return 0;
}
static inline u8 Chase_Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline void Chase_SetCtl(struct ChaseActor *actor, void *cb, u32 k)
{
    void *callback = cb;
    u32 kind = k;
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = callback;
    actor->base.field2C8 = 0;
    {
        u32 *field = &actor->base.pendingControlFlags;
        u32 mask = ~1;
        *field &= mask;
    }
    actor->base.pendingControl = 0;
}
static inline void Chase_SetState(struct ChaseActor *actor, u32 s, u32 p, u32 b)
{
    u32 behavior = b;
    actor->base.state2A8 = s;
    actor->base.phase2A9 = p;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}
static inline void Chase_SetBehavior(struct ChaseActor *actor, u32 b)
{
    actor->base.state2A8 = 0;
    actor->base.phase2A9 = 0;
    actor->base.behavior2AA = b;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}
static inline u16 Chase_MotionFlags(struct ChaseActor *actor, u32 mask)
{
    return actor->base.motionFlags & mask;
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
static inline void Chase_ClearMotionFlags(struct ChaseActor *actor, u32 bits)
{
    u32 mask = ~bits;
    actor->base.motionFlags &= mask;
}
static inline void Chase_ClearBits(u16 *field, u32 bits)
{
    *field &= ~bits;
}
static inline u8 Chase_TryRoute(struct ChaseActor *actor, void *route,
                                struct ContactPosition *position, struct ContactPosition *target)
{
    s32 result = Nav_IsPositionInMesh(route, position, target);
    if (!result) {
        Chase_ClearBits(&actor->base.motionFlags, 0x200);
        return 0;
    }
    result = Terrain_ApplyBlocked(route, position, target);
    if (result) {
        if (result != 2)
            Memory_Clear(&actor->base.body.motion, 8);
        return 1;
    }
    Chase_ClearBits(&actor->base.motionFlags, 0x200);
    return 0;
}
static inline void Chase_Delta(struct ContactPosition *out, const struct ContactPosition *from,
                               const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}
static inline s32 Chase_Scale16(s32 n)
{
    return n >= 0 ? n >> 16 : -((-n) >> 16);
}

struct ChaseFlags40 {
    s16 bits : 16;
};

struct ChasePacket {
    void *control;
    u8 state, phase, behavior, kind;
};
static inline void Chase_SavePacket(void *p, struct ChaseActor *actor)
{
    struct ChasePacket *packet = p;
    packet->control = actor->base.target;
    packet->kind = actor->base.controlKind;
    packet->state = actor->base.state2A8;
    packet->phase = actor->base.phase2A9;
    packet->behavior = actor->base.behavior2AA;
}
static inline u32 Chase_RandomBits(u32 mask)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return gRandomTable[index] & mask;
}
static inline void Chase_BeginTurn(void *obj, u8 saved)
{
    struct ChaseActor *actor = obj;
    u8 *group = (u8 *)&actor->group.common;
    if (!Chase_MotionFlags(actor, 0x400) && !Chase_Test((u16)actor->group.common.gate.flags, 4)) {
        s32 prior = actor->base.body.direction;
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
    struct ChaseActor *actor = obj;
    u8 *group = (u8 *)&actor->group.common;
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
        if (player < 0) {
        invoke:
            Callback_InvokeActor(actor, *(void **)(group + 0x224));
            goto done;
        }
        cell = ((u16 *)gPlayers[player]->followTarget)[3];
    } else {
        struct ChaseTarget *t = *slot;
        cell = Chase_CellIndex(t->position.x >> 8, t->position.z >> 8);
        player = 0;
    }
    if (cell != actor->base.terrainCell && !Chase_Bits32(&gPlayers[player]->statusFlags, 0x20000))
        goto reroute;
    if (Chase_MotionFlags(actor, 0x400))
        goto done;
    ((struct ChaseFlags40 *)(group + 0x40))->bits &= ~4;
    if (Chase_MotionFlags(actor, 0x80)) {
        Chase_SetCtl(actor, *(void **)(group + 0x48), 0x1A);
        Chase_SetBehavior(actor, 12);
    } else {
        Chase_SetCtl(actor, actor->base.onAlternateEntry, 1);
        Chase_SetBehavior(actor, 5);
        Chase_SnapToRoute(*(struct ContactPosition **)(group + 0x200), group + 0x74);
        (*(struct ContactPosition **)(group + 0x200))->y =
            Chase_HeightAt(Chase_CellAt(*(struct ContactPosition **)(group + 0x200)),
                           *(struct ContactPosition **)(group + 0x200));
        (*(struct ContactPosition **)(group + 0x200))->pad = 0;
    }
    {
        u8 saved = actor->base.body.direction;
        Chase_BeginTurn(obj, saved);
    }
    SceneActorTerrain_RequestRetreatRoute(actor);
    actor->base.field2C2 = 0x20;
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
    if (Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->base.body.position, targetPointer))
        return 1;
    Callback_InvokeActor(actor, *(void **)(group + 0x224));
}
done:
    return 0;
}

#endif
