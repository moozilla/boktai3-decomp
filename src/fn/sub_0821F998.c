#include "global.h"

/* NavMesh_StepTowardTarget: once the probe reaches its current waypoint node,
 * picks the neighbouring node with the smallest pair-table distance to the
 * target node and makes it the next waypoint (tentative). */

u16 PairTable_Lookup(u16 *table, u16 stride, s32 a, s32 b);

struct NavRegion {
    u16 f0;
    u16 stride;
    u32 f4;
    u32 edgesOffset;
    u32 nodesOffset;
    u32 distOffset;
};

struct NavNode {
    u16 x;
    u16 z;
    u16 f4;
    u8 links[6];
};

struct NavEdge {
    u32 f0;
    u8 nodes[12];
};

struct NavPair {
    u32 a, b;
};

struct NavProbe {
    u16 flags;
    u16 f2;
    u16 region;
    u16 f6;
    u16 target;
    u16 node;
    struct NavPair target8;
    u8 pad14[8];
    union {
        struct NavPair pair;
        struct {
            u16 x, y, z;
        } v;
    } way;
};

void NavMesh_StepTowardTarget(u8 *mesh, struct NavProbe *probe, s16 *pos)
{
    u32 sh = probe->region << 2;
    u32 *ov;
    struct NavRegion *region =
        (struct NavRegion *)(mesh + *(u32 *)((u32)(ov = (u32 *)(mesh + 4)) + sh));
    struct NavEdge *edges = (struct NavEdge *)((u8 *)region + region->edgesOffset);
    struct NavNode *nodes = (struct NavNode *)((u8 *)region + region->nodesOffset);
    u16 *dist = (u16 *)((u8 *)region + region->distOffset);
    struct NavNode *node = &nodes[probe->node];
    s32 dx = pos[0] - node->x;
    s32 dz = pos[2] - node->z;
    s32 best;
    s32 bestIdx;
    s32 i, j;

    if (dx * dx + dz * dz <= 0xE10) {
        if (probe->node == probe->target) {
            probe->way.pair = probe->target8;
            probe->flags = 6;
        } else {
            best = 0xFFFF;
            bestIdx = 0xFF;
            for (i = 0; i < 6 && node->links[i] != 0xFF; i++) {
                struct NavEdge *e = &edges[node->links[i]];
                for (j = 0; j < 12 && e->nodes[j] != 0xFF; j++) {
                    if (e->nodes[j] != bestIdx) {
                        u16 d = PairTable_Lookup(dist, region->stride, probe->target, e->nodes[j]);
                        if (d < best) {
                            best = d;
                            bestIdx = e->nodes[j];
                        }
                    }
                }
            }
            if (best > 0xFFFE) {
                probe->flags = 0;
            } else {
                probe->node = bestIdx;
                probe->way.v.x = nodes[bestIdx].x;
                probe->way.v.y = 0;
                probe->way.v.z = nodes[bestIdx].z;
                probe->flags |= 2;
            }
        }
    }
}
