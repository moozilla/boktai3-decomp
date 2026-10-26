#include "global.h"
#include "engine.h"
#include "contact_actor.h"

#define Navigation_IsPointInRegion NavMesh_IsPointInsideRegion
struct NavigationRegion {
    u8 x0, z0, x1, z1;
    u8 unknown4[12];
};
struct NavigationMesh {
    u16 count, unknown2;
    u32 unknown4, regionOffset;
};
struct NavigationResource {
    u16 count, unknown2;
    u32 offsets[1];
};
u32 NavMesh_IsPointInsideRegion(struct NavigationRegion *, const struct ContactPosition *, u32);
u32 Navigation_FindContainingRegion(const struct ContactPosition *position)
{
    struct NavigationResource *resource = gTerrainMap->navigation;
    s32 meshIndex, i;
    for (meshIndex = 0; meshIndex < resource->count; meshIndex++) {
        s32 off = meshIndex * 4;
        u32 *offsets = resource->offsets;
        struct NavigationMesh *mesh = (void *)((u8 *)resource + *(u32 *)((u32)offsets + off));
        struct NavigationRegion *regions = (void *)((u8 *)mesh + mesh->regionOffset);
        for (i = 0; i < mesh->count; i++) {
            if (NavMesh_IsPointInsideRegion(regions, position, i))
                return i;
        }
    }
    return 255;
}
