
#ifdef NONMATCHING
// fuzzy: 84.8%
// foundry draft (worker best 84.4%); what differs is mostly register allocation and instruction order
#define Navigation_FindContainingRegion sub_0821FCDC
#define Navigation_IsPointInRegion NavMesh_IsPointInsideRegion
#define gTerrainMap gUnk_030052F4
#include "global.h"
#include "engine.h"
#include "contact_actor.h"
struct NavigationRegion { u8 x0, z0, x1, z1; u8 unknown4[12]; };
struct NavigationMesh { u16 count, unknown2; u32 unknown4, regionOffset; };
struct NavigationResource { u16 count, unknown2; u32 offsets[1]; };
u32 NavMesh_IsPointInsideRegion(struct NavigationRegion *, const struct ContactPosition *, u32);
u32 sub_0821FCDC(const struct ContactPosition *position)
{
    struct NavigationResource *resource = gUnk_030052F4->navigation;
    s32 meshIndex = 0;
    u32 result;
    goto test;
    loop: {
        struct NavigationMesh *mesh = (void *)((u8 *)resource + *(u32 *)((meshIndex << 2) + (u32)resource->offsets));
        struct NavigationRegion *regions = (void *)((u8 *)mesh + mesh->regionOffset);
        s32 i = 0;
        for (; i < mesh->count; i++) {
            if (NavMesh_IsPointInsideRegion(regions, position, i)) { result = i; goto done; }
        }
        meshIndex++;
    }
    test:
    if (meshIndex < resource->count) goto loop;
    result = 255;
    done:
    return result;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0821FCDC);
#endif
