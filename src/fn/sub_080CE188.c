#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"

#ifdef NONMATCHING
// fuzzy: 92.8%
// fix.py best version (reprinted C)
// confidence=low: existing Golem state interpretation retained.
// confidence=low: Golem behavior table dispatches completion into chase or saved-route recovery.
// Remaining mismatch: Shared RNG loop allocation, outer group pointer lifetime and flag resets still differ.
u8 Golem_InvokeEnabledBehavior(struct SceneActorRegistryResetView *actor);
u8 SceneActorTarget_TrySelectNearestParticipant(struct SceneActorRegistryResetView *);
void sub_0807E334(struct SceneActorRegistryResetView *);
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
s32 Callback_InvokeActor(void *, void *);

extern u8 *gUnk_02000580[4];
#define PTR(a, o) (*(void **)((u8 *)(a) + (o)))
struct GolemRoutePoint {
    s16 x, z;
};
struct GolemRouteLink {
    u8 unknown0[8];
    struct GolemRoutePoint *point;
};
struct GolemFlags16 {
    u16 value;
};
#define HOME (*(struct ContactPosition **)(group + 0x200))
struct GolemTurn {
    u8 direction, savedDirection, mode;
};
struct GolemSavedControl {
    void *target;
    u8 state, phase, behavior, kind;
};

static inline u8 TakeFlag2B1(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B1) {
        actor->field2B1 = 0;
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
static inline void SetState(struct SceneActorRegistryResetView *actor, u8 state, u8 phase,
                            u8 behavior)

{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline u32 MaskFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2D4 & mask;
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
    u32 shape;
    u32 height;
    u8 *data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = (cell * 4) + 12;
        data = map->cells + offset;
    }
    shape = (*data) >> 4;
    {
        u8 k1 = 15;
        height = ((*data) & k1) << 8;
    }
    if (shape == 1)
        goto slopeZ;
    if (shape == 2)
        goto slopeX;
    goto done;
slopeZ:
    height -= *(((u8 *)position) + 4);

    goto done;
slopeX:
    height -= *((u8 *)position);

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
        ((struct GolemTurn *)(group + 0x204))->direction = RandomDirectionExcept(actor->direction);
        ((struct GolemSavedControl *)(group + 0x8C))->target = actor->target;
        ((struct GolemSavedControl *)(group + 0x8C))->kind = actor->field2B5;
        ((struct GolemSavedControl *)(group + 0x8C))->state = actor->state2A8;
        ((struct GolemSavedControl *)(group + 0x8C))->phase = actor->phase2A9;
        ((struct GolemSavedControl *)(group + 0x8C))->behavior = actor->behavior2AA;
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
        struct GolemRouteLink *link = (void *)&actor->route;
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
        cell = *(u16 *)((u8 *)PTR(gUnk_02000580[slot], 0x40) + 6);
    } else {
        cell = CellIndex((struct ContactPosition *)(*targetp + 0xC));
        slot = 0;
    }
    if (cell == actor->terrainCell || Flag32(gUnk_02000580[slot], 0x20000, 0x20)) {
        u16 busy = Flag16(actor, 0x400, 0x2D4);
        u16 blocked;
        if (busy)
            return 0;
        {
            struct GolemFlags16 *flags = (void *)(group + 0x40);
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
                struct GolemRouteLink *link = (struct GolemRouteLink *)(group + 0x74);
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

static inline void LoadRoutePoint(struct ContactPosition *dest, u8 *route)
{
    struct GolemRouteLink *link = (void *)route;
    dest->x = link->point->x;
    dest->z = link->point->z;
    dest->y = CellHeight(CellIndex(dest), dest);
}
static inline u8 RouteToBit(struct SceneActorRegistryResetView *actor, void *routeObject,
                            struct ContactPosition *position, struct ContactPosition *target,
                            u32 bit)
{
    if (!Nav_IsPositionInMesh(routeObject, position, target)) {
        actor->flags2D4 &= ~bit;
        return 0;
    } else {
        s32 result = Terrain_ApplyBlocked(routeObject, position, target);
        if (result) {
            if (result != 2)
                Memory_Clear(&actor->motion, 8);
            return 1;
        } else {
            actor->flags2D4 &= ~bit;
            return 0;
        }
    }
}
static inline void SetCommand(struct SceneActorReactionCommand *command, void *payload, u32 kind,
                              u32 duration)
{
    command->duration = duration;
    command->payload = payload;
    command->first = 0;
    command->second = 0;
    command->kind = kind;
}
void Golem_ResumeRouteAfterControl(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)(&actor->group);
    if (TakeFlag2B1(actor))
        ;
    if (Golem_InvokeEnabledBehavior(actor))
        return;
    if (actor->field2B0) {
        u16 blocked;
        SetState(actor, 3, 1, 0);
        SetControl(actor, 14, SceneActorReaction_RestoreAfterDelay);
        ProximityVolume_TranslateBounds(*((void **)(group + 0x1FC)), &actor->position);
        blocked = MaskFlags(actor, 0x80);
        if (blocked) {
            SetCommand((void *)(group + 0x8C), *((void **)(group + 0x48)), 12, 30);
            if (Test(gEngineFlags, 0x4800)) {
                if (!SceneActorTarget_TrySelectNearestParticipant(actor))
                    return;
            } else
                actor->fieldDC = (u32)(gUnk_02000580[0] + 0x24);
            if (Chase(actor))
                actor->field2C2 = 0x20;
        } else {
            u32 bit = 0x200;
            u32 *flags = &actor->flags2CC;
            if (MaskFlags(actor, bit)) {
                struct SceneActorRegistryControl *control = (void *)(group + 0x4C);
                if ((*flags) & 0x200000) {
                    actor->flags2D0 &= ~0x1000000;
                    actor->flags2D4 &= ~bit;
                    control->mode = 0;
                    control->counter = 0;
                    control->timer = 0;
                }
                SceneActorAttachment_ReplaceResponseSlot(actor, 0, 2, actor->category);
            }
            *flags |= 0x10;
            actor->field2C2 = 0x40;
            SetCommand((void *)(group + 0x8C), actor->entryTarget228, 7, 60);
            LoadRoutePoint(*((struct ContactPosition **)(group + 0x200)), group + 0x74);
            (*((struct ContactPosition **)(group + 0x200)))->pad = 1;
            if (!RouteToBit(actor, *((void **)(group + 0x1FC)), &actor->position,
                            *((struct ContactPosition **)(group + 0x200)), bit))
                Callback_InvokeActor(actor, *((void **)(group + 0x224)));
        }
    } else
        actor->counter2AC++;
}
#else
INCLUDE_ASM("asm/nonmatching", Golem_ResumeRouteAfterControl);
#endif
