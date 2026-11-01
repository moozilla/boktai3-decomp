#include "global.h"
#ifdef NONMATCHING
// fuzzy: 72.8%
// Remaining differences are helper control flow, pointer lifetimes, and register allocation.
#include "global.h"
#include "scene_actor_terrain.h"
#include "scene_actor_reaction.h"
#include "engine.h"

void Skeleton_UpdateScriptWaitState(void);
void SceneActorTerrain_RequestRetreatRoute(void *);
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
    s32 mask = ~bits;
    *field = mask & *field;
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
            Memory_Clear(&actor->body.motion, 8);
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
static inline void Chase_BeginTurn(void *obj, u8 saved, u32 duration)
{
    struct SceneActorRegistryResetView *actor = obj;
    u8 *group = (u8 *)&actor->group.common;
    if (!Chase_MotionFlags(actor, 0x400) && !Chase_Test(*(u16 *)((u8 *)actor + 0x414), 4)) {
        s32 prior = actor->body.direction;
        s32 tries = 0;
        s16 choice;
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
        group[0x206] = duration;
    }
}

static inline u8 Chase_Arrive(void *obj)
{
    struct ContactPosition target;
    struct SceneActorRegistryResetView *actor = obj;
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
        u8 saved = actor->body.direction;
        Chase_BeginTurn(obj, saved, 6);
    }
    SceneActorTerrain_RequestRetreatRoute(actor);
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
    if (Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->body.position, targetPointer))
        return 1;
invoke:
    Callback_InvokeActor(actor, *(void **)(group + 0x224));
}
done:
    return 0;
}

u8 SceneActorTerrain_CanTraverseCell(s32, s32, s32, u32);
void SceneActorControl_OnIdle(void);
static inline s32 Width(void)
{
    return *(s16 *)(gTerrainMap->cells + 4);
}
static inline u8 *Properties(s32 cell)
{
    u8 *p = TerrainModifiers_FindCellOverride(cell, 1);
    if (p)
        return p + 4;
    return gTerrainMap->cells + cell * 4 + 12;
}
static inline u16 Attributes(s32 cell)
{
    return *(u16 *)(Properties(cell) + 2);
}
static inline u8 Reach(struct SceneActorRegistryResetView *actor, s32 start, s32 target)
{
    s32 height = actor->body.position.y;
    u32 shape = *Properties(start) >> 4;
    s32 width = Width();
    s32 count = width * *(s16 *)(gTerrainMap->cells + 6);
    s32 i, column;
    if (start < target) {
        i = start + 1;
        column = Mod(i, width);
        while (column < Width()) {
            if (!SceneActorTerrain_CanTraverseCell(start, i, height, shape))
                break;
            if (i == target)
                return 1;
            i++;
            column++;
        }
        for (i = start + Width(); count > i; i += Width()) {
            if (!SceneActorTerrain_CanTraverseCell(start, i, height, shape))
                break;
            if (i == target)
                return 1;
        }
    } else {
        i = start - 1;
        column = Mod(i, width);
        while (column >= 0) {
            if (!SceneActorTerrain_CanTraverseCell(start, i, height, shape))
                break;
            if (i == target)
                return 1;
            i--;
            column--;
        }
        for (i = start - Width(); i >= 0; i -= Width()) {
            if (!SceneActorTerrain_CanTraverseCell(start, i, height, shape))
                break;
            if (i == target)
                return 1;
        }
    }
    return 0;
}
static inline void CellCenter(struct ContactPosition *out, s32 cell)
{
    s32 x = Mod(cell, Width());
    s32 z = Div(cell, Width());
    out->x = (x << 8) + 128;
    out->y = (*Properties(cell) & 15) << 8;
    out->z = (z << 8) + 128;
}
static inline u8 SameLevel(s32 cell, s32 level)
{
    if ((*Properties(cell) & 15) != level)
        return 0;
    if (Attributes(cell) & 0x43)
        return 0;
    return 1;
}
// confidence=low: finds a cardinal recovery destination and resumes route movement.
#define SceneActorMovement_SelectRecoveryRoute sub_0807AF94
u8 SceneActorMovement_SelectRecoveryRoute(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group.common;
    struct ChaseTarget *subject;
    s32 direction, cell, target;
    s32 level, width, total;
    s32 i, column;
    struct ContactPosition center;
    if (Chase_Test(*(u16 *)((u8 *)actor + 0x414), 2))
        return 0;
    if (!Chase_Test(*(u16 *)(group + 0x40), 1))
        return 0;
    subject = (void *)actor->tracking.target;
    if (actor->body.position.y != subject->position.y) {
        Chase_Arrive(actor);
        *(u16 *)(group + 0x40) |= 2;
        return 1;
    }
    switch (((actor->body.direction + 32) >> 6) & 3) {
        case 0:
        case 2:
            cell = actor->terrainCell;
            if (Reach(actor, cell, cell - Width())) {
                direction = 3;
                break;
            }
            cell = actor->terrainCell;
            if (Reach(actor, cell, cell + Width())) {
                direction = 1;
                break;
            }
            direction = -1;
            break;
        case 1:
        case 3:
            cell = actor->terrainCell;
            if (Reach(actor, cell, cell + 1)) {
                direction = 0;
                break;
            }
            cell = actor->terrainCell;
            if (Reach(actor, cell, cell - 1)) {
                direction = 2;
                break;
            }
            direction = -1;
            break;
        default:
            direction = -1;
            break;
    }
    if (direction < 0)
        return 0;
    cell = actor->terrainCell;
    level = *Properties(cell) & 15;
    width = Width();
    total = width * *(s16 *)(gTerrainMap->cells + 6);
    switch (direction) {
        case 0:
            i = cell + 1;
            column = Mod(i, width);
            while (column < Width()) {
                if (!SameLevel(i, level)) {
                    target = i - 1;
                    goto found;
                }
                i++;
                column++;
            }
            break;
        case 1:
            for (i = cell + width; total > i; i += Width()) {
                if (!SameLevel(i, level)) {
                    target = i - Width();
                    goto found;
                }
            }
            break;
        case 2:
            i = cell - 1;
            column = Mod(i, width);
            while (column >= 0) {
                if (!SameLevel(i, level)) {
                    target = i + 1;
                    goto found;
                }
                i--;
                column--;
            }
            break;
        case 3:
            for (i = cell - width; i >= 0; i -= Width()) {
                if (!SameLevel(i, level)) {
                    target = i + Width();
                    goto found;
                }
            }
            break;
    }
    target = cell;
found:
    if (!target || target == actor->terrainCell)
        return 0;
    Chase_BeginTurn(actor, actor->body.direction, 2);
    CellCenter(&center, target);
    center.y = actor->body.position.y;
    if (Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->body.position, &center)) {
        *(u16 *)(group + 0x40) |= 2;
        return 1;
    }
    Chase_SetCtl(actor, SceneActorControl_OnIdle, 0);
    Chase_SetBehavior(actor, 3);
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0807AF94);
#endif
