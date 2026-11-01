#include "global.h"
#include "engine.h"
#include "nav_mesh.h"

u32 Navigation_FindContainingRegion(const struct ContactPosition *position)
{
    struct NavMeshResource *resource = gTerrainMap->navigation;
    s32 meshIndex, i;
    for (meshIndex = 0; meshIndex < resource->count; meshIndex++) {
        s32 off = meshIndex * 4;
        u32 *offsets = resource->offsets;
        struct NavMeshGraph *mesh = (void *)((u8 *)resource + *(u32 *)((u32)offsets + off));
        struct NavMeshRegion *regions = (void *)((u8 *)mesh + mesh->regionOffset);
        for (i = 0; i < mesh->count; i++) {
            if (NavMesh_IsPointInsideRegion(regions, position, i))
                return i;
        }
    }
    return 255;
}
