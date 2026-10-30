#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
#ifdef NONMATCHING
// fuzzy: 88.5%
// fix.py best version (reprinted C)
// confidence=low: inherited route/reaction interpretation from static callers.

// Saved actor lifetimes, control offsets, and duplicated route-reset allocation still differ.
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

static inline void ClearRouteBits(u16 *field, u32 bits)
{
    *field &= ~bits;
}
static inline u8 TryRoute(struct SceneActorRegistryResetView *actor, u8 *group,
                          struct ContactPosition *target)
{
    void *route = *(void **)(group + 0x1fc);
    struct ContactPosition *position = &actor->body.position;
    s32 result = Nav_IsPositionInMesh(route, position, target);
    if (result) {
        result = Terrain_ApplyBlocked(route, position, target);
        if (result) {
            if (result != 2)
                Memory_Clear(&actor->body.motion, 8);
            return 1;
        }
        ClearRouteBits(&actor->flags2D4, 0x200);
        return 0;
    }
    ClearRouteBits(&actor->flags2D4, 0x200);
    return 0;
}

static inline void SetControl(struct SceneActorRegistryResetView *actor, void *cb, u32 k)
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
static inline void SetState(struct SceneActorRegistryResetView *actor, u32 b)
{
    u32 behavior = b;
    actor->state2A8 = 0;
    actor->phase2A9 = 0;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

// confidence=low: restores route after context response flags.
void Vorn_TickSound(void *);
void Vorn_SendMessage(void *);
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
void SceneBody_StartRoute(void *, void *, u32, u32, u32);
static inline u32 Mask16(struct SceneActorRegistryResetView *p, u32 mask)
{
    return p->flags2D4 & mask;
}
static inline u32 Mask32(struct SceneActorRegistryResetView *p, u32 mask)
{
    return p->flags2D0 & mask;
}
static inline u8 IsInactive(struct SceneActorRegistryResetView *p)
{
    if (p && (Mask16(p, 0x40) || Mask32(p, 0x20000) || p->health <= 0))
        return 1;
    return 0;
}
static inline u8 Test(u16 *p, u32 mask)
{
    if (*p & mask)
        return 1;
    return 0;
}
static inline u8 TestBody(void *p, u32 mask)
{
    if (((struct SceneActorRegistryResetView *)p)->flags6D4 & mask)
        return 1;
    return 0;
}
static inline u32 MaskLow(struct SceneActorRegistryResetView *p, u32 mask)
{
    return p->flags2CC & mask;
}
static inline void SnapTarget(struct ContactPosition *out, u16 *point)
{
    out->x = point[0];
    out->z = point[1];
    out->y = CellHeight(out, CellIndex(out));
}
static inline void Resume(struct SceneActorRegistryResetView *input, u8 *group, u32 *flags)
{
    struct SceneActorRegistryResetView *actor = input;
    struct ContactPosition target;
    struct RouteLink {
        u8 pad[8];
        u16 *point;
    };
    struct RouteLink *point;
    actor->field2C2 = 64;
    actor->group.common.gate.flags = 0;
    actor->group.common.gate.counter = 0;
    ClearRouteBits(&actor->flags2D4, 0x380);
    {
        struct SceneActorReactionMotion *motion = (void *)(((u8 *)actor) + 0xdc);
        u32 mask = ~0x18;
        u32 value = motion->flags;
        value &= mask;
        motion->flags = value;
    }
    {
        struct SceneActorRegistryControl *control = (void *)(group + 0x4c);
        if (MaskLow(actor, 0x200000)) {
            *flags &= 0xfeffffff;
            ClearRouteBits(&actor->flags2D4, 0x200);
            {
                control->mode = 0;
                control->counter = 0;
                control->timer = 0;
            }
        }
    }
    ClearRouteBits(&actor->flags2D4, 0x400);
    actor->group.turn.turnsRemaining = 255;
    SceneActorAttachment_ReplaceResponseSlot(actor, 0, 2, 0x19);
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
    {
        u8 k1 = 3;
        actor->state2A8 = k1;
    }
    actor->phase2A9 = 1;
    actor->behavior2AA = 0;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
    {
        u8 *command = group + 0x8c;
        u32 target = (u32)actor->entryTarget228;
        u32 kind = 7;
        *((u32 *)(command + 12)) = 60;
        *((u32 *)(command + 16)) = target;
        command[8] = 0;
        command[9] = 0;
        command[10] = kind;
    }
    {
        u32 mask = 16;
        u32 *field = &actor->flags2CC;
        u32 value = *field;
        value |= mask;
        *field = value;
    }
    ClearRouteBits(&actor->flags2D6, 0x9c9);
    SceneBody_StartRoute(&actor->body.id, group + 0x74, group[0x84], group[0x85], group[0x86]);
    point = (struct RouteLink *)(group + 0x74);
    SnapTarget(&target, point->point);
    (*((struct ContactPosition **)(group + 0x200)))->pad = 1;
    if (!TryRoute(actor, group, &target))
        Callback_InvokeActor(actor, *((void **)(group + 0x224)));
}

void Vorn_ResumeRouteAfterReaction(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)(&actor->group.common);
    u8 *body;
    u32 *flags = &actor->flags2D0;
    if (IsInactive(actor))
        return;
    if (Mask16(actor, 0xe33f))
        return;
    if (!Mask16(actor, 0x80))
        return;
    {
        u32 mask = 4;
        u32 *field = &actor->flags2D0;
        u32 value = *field;
        value |= mask;
        *field = value;
    }
    body = actor->context;
    if (*((u16 *)(group + 0x44))) {
        if ((actor->field2B5 == 0x25) && TestBody(body, 4))
            Vorn_TickSound(actor);
        return;
    }
    if (TestBody(body, 4)) {
        Resume(actor, group, flags);
        ClearRouteBits((u16 *)(((u8 *)actor->context) + 0x6d4), 6);
        *flags &= ~4;
        Vorn_SendMessage(actor);
    } else if (TestBody(body, 16)) {
        Resume(actor, group, flags);
        ClearRouteBits((u16 *)(((u8 *)actor->context) + 0x6d4), 0x70);
        *flags &= ~4;
        Vorn_SendMessage(actor);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", Vorn_ResumeRouteAfterReaction);
#endif
