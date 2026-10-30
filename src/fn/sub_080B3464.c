#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_delayed_response.h"

static inline u32 Vorn_CellIndex(struct ContactPosition *p)
{
    s32 x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline u32 Vorn_CellHeight(struct ContactPosition *position, u32 cell)
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
    height -= (u8)position->z;
    goto done;
slopeX:
    height -= (u8)position->x;
done:
    return height;
}

static inline void Vorn_ClearRouteBits(u16 *field, u32 bits)
{
    *field &= ~bits;
}
static inline void Vorn_SetControl(struct SceneActorRegistryResetView *actor, void *cb, u32 k)
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
/* Restores the saved route after a context response; interpretation is tentative. */
void Vorn_TickSound(void *);
void Vorn_SendMessage(void *);
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
void SceneBody_StartRoute(void *, void *, u32, u32, u32);
static inline u32 Vorn_Mask16(struct SceneActorRegistryResetView *p, u32 mask)
{
    return p->flags2D4 & mask;
}
static inline u32 Vorn_Mask32(struct SceneActorRegistryResetView *p, u32 mask)
{
    return p->flags2D0 & mask;
}
static inline u8 Vorn_IsInactive(struct SceneActorRegistryResetView *p)
{
    if (p && (Vorn_Mask16(p, 0x40) || Vorn_Mask32(p, 0x20000) || p->health <= 0))
        return 1;
    return 0;
}
static inline u8 Vorn_TestBody(void *p, u32 mask)
{
    if (((struct SceneActorRegistryResetView *)p)->flags6D4 & mask)
        return 1;
    return 0;
}
static inline u32 Vorn_MaskLow(struct SceneActorRegistryResetView *p, u32 mask)
{
    return p->flags2CC & mask;
}
static inline void Vorn_SnapTarget(struct ContactPosition *out,
                                   struct SceneActorChaseRoutePoint *point)
{
    out->x = point->x;
    out->z = point->z;
    out->y = Vorn_CellHeight(out, Vorn_CellIndex(out));
}
static inline void Vorn_Resume(struct SceneActorRegistryResetView *input,
                               struct SceneActorChaseGroup *group, u32 *flags)
{
    struct SceneActorRegistryResetView *actor = input;
    struct ContactPosition target;
    struct SceneActorRegistryRoute *point;
    actor->field2C2 = 64;
    actor->group.common.gate.flags = 0;
    actor->group.common.gate.counter = 0;
    Vorn_ClearRouteBits(&actor->flags2D4, 0x380);
    {
        struct SceneActorChaseTracking *motion = &actor->tracking;
        u32 mask = ~0x18;
        motion->flags = mask & motion->flags;
    }
    {
        struct SceneActorRegistryControl *control = &actor->group.common.control;
        if (Vorn_MaskLow(actor, 0x200000)) {
            SceneActorChase_ClearWordFlags(flags, 0x1000000);
            Vorn_ClearRouteBits(&actor->flags2D4, 0x200);
            {
                control->mode = 0;
                control->counter = 0;
                control->timer = 0;
            }
        }
    }
    Vorn_ClearRouteBits(&actor->flags2D4, 0x400);
    actor->group.turn.turnsRemaining = 255;
    SceneActorAttachment_ReplaceResponseSlot(actor, 0, 2, 0x19);
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    Vorn_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
    {
        u8 k1 = 3;
        actor->state2A8 = k1;
    }
    actor->phase2A9 = 1;
    actor->behavior2AA = 0;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
    {
        struct SceneActorDelayedResponse *command = (void *)&group->saved;
        u32 target = (u32)actor->entryTarget228;
        u32 kind = 7;
        command->delay = 60;
        command->control = (void *)target;
        command->state = 0;
        command->phase = 0;
        command->behavior = kind;
    }
    {
        u32 mask = 16;
        u32 *field = &actor->flags2CC;
        u32 value = *field;
        value |= mask;
        *field = value;
    }
    Vorn_ClearRouteBits(&actor->flags2D6, 0x9c9);
    SceneBody_StartRoute(&actor->body.id, &group->homeRoute, group->homeRoute.routeId,
                         group->homeRoute.parameterB, group->homeRoute.pointId);
    point = &group->homeRoute;
    Vorn_SnapTarget(&target, point->point);
    group->destination->pad = 1;
    if (!SceneActorChase_TryRoute(actor, group->route, &input->body.position, &target))
        Callback_InvokeActor(input, group->fallback);
}

void Vorn_ResumeRouteAfterReaction(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    struct SceneActorChaseGroup *group = &actor->group;
    struct SceneActorRegistryResetView *body;
    u32 *flags;
    u32 gateCounter;
    if (Vorn_IsInactive(actor))
        return;
    if (Vorn_Mask16(actor, 0xe33f))
        return;
    if (!Vorn_Mask16(actor, 0x80))
        return;
    {
        u32 mask = 4;
        u32 *field = &actor->flags2D0;
        u32 value = *field;
        value |= mask;
        *field = value;
    }
    body = actor->context;
    gateCounter = group->common.gate.counter;
    flags = &actor->flags2D0;
    if (gateCounter) {
        if ((actor->field2B5 == 0x25) && Vorn_TestBody(body, 4))
            Vorn_TickSound(object);
        return;
    }
    if (Vorn_TestBody(body, 4)) {
        Vorn_Resume(actor, group, flags);
        Vorn_ClearRouteBits(&((struct SceneActorRegistryResetView *)actor->context)->flags6D4, 6);
        SceneActorChase_ClearWordFlags(flags, 4);
        Vorn_SendMessage(actor);
    } else if (Vorn_TestBody(body, 16)) {
        Vorn_Resume(actor, group, flags);
        Vorn_ClearRouteBits(&((struct SceneActorRegistryResetView *)actor->context)->flags6D4,
                            0x70);
        SceneActorChase_ClearWordFlags(flags, 4);
        Vorn_SendMessage(actor);
    }
}
