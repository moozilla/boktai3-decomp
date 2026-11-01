#include "nav_mesh.h"

u32 NavMesh_IsPointInsideRegion(struct NavMeshRegion *r, const struct ContactPosition *pt, u32 i)
{
    struct NavMeshRegion *e = r + i;
    s32 x = pt->x;
    if (x >= (e->x0 << 8) && x < (e->x1 << 8)) {
        s32 y = pt->z;
        if (y >= (e->z0 << 8) && y < (e->z1 << 8))
            return 1;
    }
    return 0;
}
