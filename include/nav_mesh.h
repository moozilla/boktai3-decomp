#ifndef GUARD_NAV_MESH_H
#define GUARD_NAV_MESH_H

#include "contact_position.h"

struct NavMeshResource {
    u16 count, unknown2;
    u32 offsets[1];
};
struct NavMeshGraph {
    u16 count, nodeCount;
    u32 unknown4;
    u32 regionOffset, nodeOffset, distanceOffset;
};
struct NavMeshRegion {
    u8 x0, z0, x1, z1;
    u8 nodes[12];
};
struct NavMeshNode {
    u16 x, z, unknown4;
    u8 regions[6];
};

u32 NavMesh_IsPointInsideRegion(struct NavMeshRegion *, const struct ContactPosition *, u32);
u16 PairTable_Lookup(u16 *, u16, s32, s32);

#endif
