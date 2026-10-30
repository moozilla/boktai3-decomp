#include "global.h"
#include "scene_actor_reaction.h"
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
struct RouteResponseGroup {
    u8 unknown0[0x74];
    struct RoutePointView {
        u8 unknown0[8];
        const u16 *point;
    } route;
    u8 unknown80[0xC];
    struct RouteResponse {
        u8 unknown0[8];
        u8 state, phase, behavior, unknownB;
        s32 threshold;
        void *control;
    } response;
    u8 unknownA0[0x1FC - 0xA0];
    void *context;
    struct ContactPosition *destination;
};
static inline u32 CellBits(const u8 *data, u32 mask)
{
    u32 bits = mask;
    bits &= *data;
    return bits;
}
static inline u8 CoordinateFraction(s16 coordinate)
{
    return coordinate;
}
static inline u32 TerrainHeight(struct ContactPosition *position)
{
    s32 x = position->x >> 8, z = position->z >> 8;
    u32 cell, flags, shape, height;
    u8 *data;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        cell = 0;
    else
        cell = gTerrainMap->rows[z] + x;
    data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = cell * 4 + 12;
        data = map->cells + offset;
    }
    flags = *data;
    shape = flags >> 4;
    height = CellBits(data, 15) << 8;
    switch (shape) {
        case 1:
            height -= CoordinateFraction(position->z);
            break;
        case 2:
            height -= CoordinateFraction(position->x);
            break;
    }
    return height;
}
static inline void SnapToRoute(struct ContactPosition *position, struct RoutePointView *route)
{
    position->x = route->point[0];
    position->z = route->point[1];
    position->y = TerrainHeight(position);
}
static inline void ClearBits(u16 *flags, u32 bits)
{
    *flags &= ~bits;
}
static inline void SetActorBits(struct SceneActorRegistryResetView *actor, u32 bits)
{
    actor->flags2CC |= bits;
}
static inline u8 TryRoute(struct SceneActorRegistryResetView *actor, void *context,
                          struct ContactPosition *source, struct ContactPosition *destination)
{
    s32 result;
    if (!Nav_IsPositionInMesh(context, source, destination)) {
        ClearBits(&actor->flags2D4, 0x200);
        return 0;
    }
    result = Terrain_ApplyBlocked(context, source, destination);
    if (result) {
        if (result != 2)
            Memory_Clear(&actor->body.motion, 8);
        return 1;
    }
    ClearBits(&actor->flags2D4, 0x200);
    return 0;
}
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *callback, u32 kind)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = callback;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        u32 mask = ~1;
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
static inline void SetResponse(struct RouteResponse *response, void *control, u32 state, u32 phase,
                               u32 behavior, s32 threshold)
{
    response->threshold = threshold;
    response->control = control;
    response->state = state;
    response->phase = phase;
    response->behavior = behavior;
}
void SceneActorReaction_TryResumeRoute(struct SceneActorRegistryResetView *actor, s32 frame)
{
    struct RouteResponseGroup *group = (void *)&actor->group;
    if (SceneActorReaction_TakePrepared(actor)) {
        actor->body.direction = (((actor->field2B6 + 3) & 7) << 5) - 64;
        SceneActorAnimation_SelectDirectionalEntry((struct SceneActorAnimationView *)actor, 0);
        actor->field2B3 = 0;
    }
    if (frame > 60 && actor->field2B3) {
        actor->field2B0 = 1;
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 2, actor->category);
        SnapToRoute(group->destination, &group->route);
        if (!TryRoute(actor, group->context, &actor->body.position, group->destination)) {
            SetControl(actor, SceneActorControl_OnIdle, 0);
            SetState(actor, 3, 0, 0);
        } else {
            group->destination->pad = 1;
            SetState(actor, 3, 1, 0);
            SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
            SetActorBits(actor, 0x10);
            actor->field2C2 = 64;
            SetResponse(&group->response, actor->entryTarget228, 0, 0, 7, 60);
        }
    }
}
