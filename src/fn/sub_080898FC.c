#include "global.h"
#ifdef NONMATCHING
// fuzzy: 87.1%
// fix.py best version (reprinted C)
// Remaining differences include shared route tails, flag/address lifetimes, and register allocation.
/* Local chase helpers follow scene_actor_chase.h; the RNG mask pin reproduces
 * this caller's retry-loop allocation without changing the shared header. */
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

struct ChaseContext {
    u8 unknown0[0x6DC];
    u32 flags;
    u8 unknown6E0[0x28];
    u32 nearDistance;
    u32 farDistance;
};

static inline u8 Chase_TakeArrived(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B1) {
        actor->field2B1 = 0;
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
static inline void Chase_ClearMotionFlags(struct SceneActorRegistryResetView *actor, u32 bits)
{
    u32 mask = ~bits;
    actor->flags2D4 &= mask;
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
static inline void Chase_SavePacket(void *p, struct SceneActorRegistryResetView *actor)
{
    struct ChasePacket *packet = p;
    packet->control = actor->target;
    packet->kind = actor->field2B5;
    packet->state = actor->state2A8;
    packet->phase = actor->phase2A9;
    packet->behavior = actor->behavior2AA;
}
static inline u32 Chase_RandomBits(u32 input)
{
    register u32 mask asm("r7") = input;
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
        if (player < 0) {
        invoke:
            Callback_InvokeActor(actor, *(void **)(group + 0x224));
            goto done;
        }
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
    Callback_InvokeActor(actor, *(void **)(group + 0x224));
}
done:
    return 0;
}

// MATCHING_CONCESSION: register-pinning
// confidence=low: waits sixty updates, then restores chase or saved-route behavior after a response change.
u8 sub_0812F3CC(struct SceneActorRegistryResetView *);
struct GhoulResumeAction {
    u8 pad[8];
    u8 state, phase, behavior, padB;
    u32 duration;
    void *target;
};
static inline void SaveAction(struct GhoulResumeAction *input, void *callback, u32 kind)
{
    struct GhoulResumeAction *action = input;
    register void *target asm("r2") = callback;
    register u32 behavior asm("r3") = kind;
    register u32 duration asm("r0") = 30;
    asm("" : "+r"(action), "+r"(target), "+r"(behavior));
    action->duration = duration;
    action->target = target;
    action->state = 0;
    action->phase = 0;
    action->behavior = behavior;
}
void Ghoul_ResumeDelayedChase(struct SceneActorRegistryResetView *actor)
{
    u8 *group = ((u8 *)actor) + 0x3D4;
    typeof(*((void **)(group + 0x1FC))) t1;
    typeof(&actor->position) t2;
    typeof(*((struct ContactPosition **)(group + 0x200))) t3;
    asm("" : "+r"(group));
    {
        register u8 *entry asm("r1") = &actor->field2B1;
        asm("" : "+r"(entry));
        if (*entry)
            *entry = 0;
    }
    if ((u8)Callback_InvokeActor(actor, *((void **)(((u8 *)actor) + 0x270))))
        return;
    if (sub_0812F3CC(actor))
        return;
    if (((s32)actor->counter2AC) <= 60)
        goto increment;
    SceneActorAttachment_ReplaceResponseSlot(actor, 0, 2, actor->category);
    Chase_SetState(actor, 3, 1, 0);
    Chase_SetCtl(actor, SceneActorReaction_RestoreAfterDelay, 14);
    if (Chase_MotionFlags(actor, 0x80)) {
        SaveAction((void *)(group + 0x8C), *((void **)(group + 0x48)), 12);
        Chase_Arrive(actor);
    } else if (Chase_Bits32(&actor->flags2CC, 0x10)) {
        struct ContactPosition *destination;
        SaveAction((void *)(group + 0x8C), actor->entryTarget228, 7);
        destination = *((struct ContactPosition **)(group + 0x200));
        Chase_SnapToRoute(destination, group + 0x74);
        t1 = *((void **)(group + 0x1FC));
        t2 = &actor->position;
        t3 = *((struct ContactPosition **)(group + 0x200));
        if (!Chase_TryRoute(actor, t1, t2, t3)) {
            Chase_SetCtl(actor, SceneActorControl_OnIdle, 0);
            Chase_SetState(actor, 3, 0, 0);
        } else
            (*((struct ContactPosition **)(group + 0x200)))->pad = 1;
    } else {
        SaveAction((void *)(group + 0x8C), actor->entryTarget228, 2);
        actor->field2C2 = 64;
    }
    return;
increment:
    actor->counter2AC++;
}
#else
INCLUDE_ASM("asm/nonmatching", Ghoul_ResumeDelayedChase);
#endif
