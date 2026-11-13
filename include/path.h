#ifndef GUARD_PATH_H
#define GUARD_PATH_H
#include "scene_actor_route.h"
#include "terrain_cell_probe.h"
#include "script_reference.h"
struct PathResource {
    u16 count, unknown2;
    struct SceneActorRouteDefinition definitions[1];
};
struct SceneActorRouteDefinition *Path_GetDefinition(u32 id);
struct SceneActorRoutePoint *Path_GetPointData(struct SceneActorRouteDefinition *definition);
void Path_GetPointPosition(struct Pos16 *out, struct SceneActorRoutePoint *points, u32 index);
u32 Path_GetPointFacing(struct SceneActorRoutePoint *points, u32 index);
u32 Path_GetPointNodeId(struct SceneActorRoutePoint *points, u32 index);
s32 Path_GetPointPositionById(struct Pos16 *out, u32 path, u32 point);
s32 Path_PickRandomPoint(struct Pos16 *out, u32 path);
s32 Path_SnapToNearestPoint(struct Pos16 *out, struct Pos16 *reference, u32 path,
                            s32 unusedDistance);
s32 Path_SnapToFarthestPoint(struct Pos16 *out, struct Pos16 *reference, u32 path,
                             s32 unusedDistance);
s32 Path_FindNearestDistantPoint(struct ContactPosition *out, struct ContactPosition *reference,
                                 u32 path, u32 minimum);
s32 Path_SnapToRandomFarPoint(struct Pos16 *out, struct Pos16 *reference, u32 path, u32 minimum);
s32 Path_SnapToRandomPointInBox(struct Pos16 *out, struct Pos16 *lo, struct Pos16 *hi, u32 path);
#endif
