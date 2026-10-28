// MATCHING_CONCESSION: register-pinning
#include "global.h"
#ifdef NONMATCHING
// fuzzy: 84.7%
// Golem/skeleton reaction dispatcher (returns 1 when it changed control): hit (0x2000) turns
// round and chases, 0xC0000 hands over to reactionTarget230, otherwise roar-and-chase through
// three copies of Chase_Arrive; 080C23FC is its far tail.
// Remaining differences include helper address lifetimes and turn-loop allocation.
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

struct ChaseContext {
    u8 unknown0[0x6DC];
    u32 flags;
    u8 unknown6E0[0x28];
    u32 nearDistance;
    u32 farDistance;
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
                                struct ContactPosition *position,
                                struct ContactPosition *destination)
{
    s32 result;
    if (!Nav_IsPositionInMesh(route, position, destination)) {
        Chase_ClearBits(&actor->flags2D4, 0x200);
        return 0;
    } else {
        result = Terrain_ApplyBlocked(route, position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear(&actor->motion, 8);
            return 1;
        } else {
            Chase_ClearBits(&actor->flags2D4, 0x200);
            return 0;
        }
    }
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
static inline u8 Chase_Test16(u16 *p, u32 mask)
{
    if (*p & mask)
        return 1;
    return 0;
}
static inline u32 PickTurn(u32 dir)
{
    register s32 prior asm("r7") = dir;
    s32 tries = 0;
    s32 choice;
    do {
        {
            register u32 bits asm("r1") = 3;
            u32 index = (gRandomIndex + 1) & 1023;
            u32 v;
            gRandomIndex = index;
            index *= 2;
            v = *(const u16 *)(index + (u32)gUnk_0203B400);
            choice = (v & bits) << 6;
        }
        if (choice != prior)
            goto chosen;
    } while (++tries <= 3);
    choice = prior;
chosen:
    return choice;
}
static inline void Chase_BeginTurn(void *obj, u8 saved)
{
    struct SceneActorRegistryResetView *actor = obj;
    u8 *group = (u8 *)&actor->group;
    u32 mask = 0x400;
    if (actor->flags2D4 & mask)
        return;
    if (Chase_Test16((u16 *)((u8 *)obj + 0x414), 4))
        return;
    group[0x204] = PickTurn(actor->direction);
    Chase_SavePacket(group + 0x8C, actor);
    group[0x205] = saved;
    Chase_SetCtl(actor, *(void **)(group + 0x21C), 0x1B);
    Chase_SetState(actor, 3, 1, 0);
    group[0x206] = 6;
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
void SceneActorScript_TriggerCallOnCount(struct SceneActorRegistryResetView *);

static inline u8 StartRoar(struct SceneActorRegistryResetView *actor)
{
    u8 *roar = (u8 *)actor + 0x5CA;
    if (*roar == 0 && (*(u16 *)((u8 *)actor + 0x2E6) & 8)) {
        actor->field2B8 = 0x2D;
        *roar = 0x41;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
        return 1;
    }
    return 0;
}
static inline void ChasePlayer(struct SceneActorRegistryResetView *actor)
{
    actor->fieldDC = (u32)((u8 *)gPlayers[0] + 0x24);
}

// confidence=low: contact flags select chase, roar, or a reaction control.
u8 Golem_ReactToContact(struct SceneActorRegistryResetView *actor)
{
    u8 *body = actor->context;
    u8 *group = (u8 *)&actor->group;
    u16 flags;

    if (!(actor->field2D8 & 0x20000)) {
        if (*(u16 *)((u8 *)actor + 0x2E4) == 0)
            Sound_PlaySong(0x211);
        else
            Sound_PlaySong(0x13E);
    }
    SceneActorScript_TriggerCallOnCount(actor);
    {
        u32 mask = 0x2000;
        flags = actor->flags2D4;
        if (!(flags & mask))
            goto otherContact;
    }
    {
        actor->direction = *((u8 *)actor + 0x2B6) + 0x80;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        actor->flags2D4 &= ~0x2020;
        actor->flags2CC |= 0x10;
        (*(struct ContactPosition **)(group + 0x200))->pad = 0;
        ChasePlayer(actor);
        if (!Chase_Arrive(actor)) {
            if (!Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->position,
                                &((struct ChaseTarget *)actor->fieldDC)->position)) {
                Callback_InvokeActor(actor, *(void **)(group + 0x224));
                return 0;
            }
        }
        Chase_SetCtl(actor, actor->entryTarget228, 1);
        Chase_SetBehavior(actor, 7);
        actor->field2C2 = Div(0x40, 3);
        return 1;
    }
otherContact:
    if (flags & 0x8002) {
        actor->flags2D4 = flags & ~0x20;
        return 0;
    }
    if (*(u32 *)((u8 *)actor + 0x2DC) & 0xC0000) {
        Chase_SetCtl(actor, actor->reactionTarget230, 0x12);
        Chase_SetState(actor, 3, 0, 0);
        actor->flags2D0 &= ~0x1000000;
        return 1;
    }
    flags &= ~0x20;
    actor->flags2D4 = flags;
    if (flags & 0x100)
        return 0;
    if (actor->field2B8)
        return 0;
    if (Chase_Test(actor->flagsEB, 0x10))
        return 0;
    if (actor->flags2D4 & 0x80) {
        if (StartRoar(actor))
            Sound_PlaySong(0x3A7);
        if (!actor->fieldDC)
            return 0;
        actor->flags2CC |= 0x80;
        *(u16 *)(group + 0x40) |= 4;
        ChasePlayer(actor);
        if (!Chase_Arrive(actor)) {
            *(u16 *)(body + 0x6F0) = 0;
            if (!Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->position,
                                &((struct ChaseTarget *)actor->fieldDC)->position)) {
                Callback_InvokeActor(actor, *(void **)(group + 0x224));
                return 0;
            }
        }
        Chase_SetCtl(actor, *(void **)(group + 0x48), 0x1A);
        Chase_SetBehavior(actor, 12);
        actor->field2C2 = 0x20;
        return 1;
    }
    if (StartRoar(actor))
        Sound_PlaySong(0x3A7);
    if (!actor->fieldDC)
        return 0;
    ChasePlayer(actor);
    actor->flags2CC |= 0x10;
    (*(struct ContactPosition **)(group + 0x200))->pad = 0;
    if (!Chase_Arrive(actor)) {
        *(u16 *)(body + 0x6F0) = 0;
        if (!Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->position,
                            &((struct ChaseTarget *)actor->fieldDC)->position)) {
            Callback_InvokeActor(actor, *(void **)(group + 0x224));
            return 0;
        }
    }
    Chase_SetCtl(actor, actor->entryTarget228, 1);
    Chase_SetBehavior(actor, 7);
    actor->field2C2 = 0x20;
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", Golem_ReactToContact);
#endif
