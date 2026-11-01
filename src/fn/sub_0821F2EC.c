#include "engine.h"
#include "nav_mesh.h"

struct NavMeshRoute {
    u16 status;
    u8 unk2[2];
    u16 mesh;
    u16 region;
    u16 targetNode;
    u16 waypointNode;
    struct ContactPosition destination;
    struct ContactPosition origin;
    struct ContactPosition waypoint;
};

#define NAV_MESH_GET_NODE(out, nodes, region, slot)                                                \
    do {                                                                                           \
        (out) = &(nodes)[(region)->nodes[(slot)]];                                                 \
    } while (0)

#define NAV_MESH_GATHER_LINK_MEMBERS(node, regionTable, position, found, count, j, t)              \
    for (j = 0; j <= 5 && node->regions[j] != 0xFF && count <= 5; j++) {                           \
        if (node->regions[j] != found[0] &&                                                        \
            NavMesh_IsPointInsideRegion(regionTable, position, node->regions[j])) {                \
            s32 isNew = 1;                                                                         \
            for (t = 0; t < count; t++) {                                                          \
                if (found[t] == node->regions[j])                                                  \
                    isNew = 0;                                                                     \
            }                                                                                      \
            if (isNew) {                                                                           \
                found[count] = node->regions[j];                                                   \
                count++;                                                                           \
            }                                                                                      \
        }                                                                                          \
    }
#define NAV_MESH_COLLECT_NEIGHBORS(node, regionTable, position, found, count, j, t)                \
    do {                                                                                           \
        NAV_MESH_GATHER_LINK_MEMBERS(node, regionTable, position, found, count, j, t);             \
    } while (0)

/* Advance from the slot that supplied the current destination node. */
static inline s32 NavMesh_NextNodeSlot(s32 *currentSlot, s32 slot)
{
    *currentSlot = slot;
    return *currentSlot + 1;
}

/* Returns 2 for the cached endpoints, 1 for a route, or 0 if no route exists. */
u32 NavMesh_FindRoute(void *route, void *origin, void *destination)
{
    s32 i;
    struct NavMeshRoute *state = route;
    const struct ContactPosition *from = origin;
    const struct ContactPosition *to = destination;
    struct NavMeshResource *table;
    struct NavMeshGraph *mesh;
    struct NavMeshRegion *regions;
    struct NavMeshRegion *destinationRegion;
    struct NavMeshNode *nodes;
    u16 *dist;
    struct NavMeshRegion *originRegion;
    u16 destinationRegions[6];
    u16 originRegions[6];
    u32 shortest, waypointNode, targetNode;
    u16 destinationCount;
    u16 originCount;
    s32 meshId;
    s32 regionId;
    s32 j, k, t, u, v;

    if (state->destination.x == to->x && state->destination.z == to->z &&
        state->origin.x == from->x && state->origin.z == from->z)
        return 2;
    table = gTerrainMap->navigation;
    state->destination.x = to->x;
    state->destination.z = to->z;

    meshId = 0xFF;
    regionId = 0xFF;
    mesh = 0;
    regions = 0;

    for (i = 0; i < table->count; i++) {
        u32 shift = i << 2;
        u32 *offsets = table->offsets;
        mesh = (void *)((u8 *)table + *(u32 *)((u32)offsets + shift));
        regions = (void *)((u8 *)mesh + mesh->regionOffset);
        for (k = 0; k < mesh->count; k++) {
            if (NavMesh_IsPointInsideRegion(regions, to, k)) {
                meshId = i;
                regionId = k;
                /* Stop at the first region containing the destination. */
                i = table->count;
                k = mesh->count;
            }
        }
    }

    if (regionId == 0xFF) {
        state->status = 0;
        return 0;
    }
    if (state->mesh != meshId) {
        state->status = 0;
        return 0;
    }
    if (state->region == regionId) {
        state->waypoint = state->destination = *to;
        state->origin = *from;
        state->status = 6;
        return 1;
    }
    if (mesh == 0 || regions == 0)
        return 0;

    nodes = (void *)((u8 *)mesh + mesh->nodeOffset);
    dist = (void *)((u8 *)mesh + mesh->distanceOffset);
    for (t = 0; t < 6; t++)
        destinationRegions[t] = 0xFF;

    destinationRegion = &regions[regionId];
    destinationRegions[0] = regionId;
    destinationCount = 1;
    {
        s32 currentSlot;
        for (i = 0; i <= 11 && destinationRegion->nodes[i] != 0xFF && destinationCount <= 5;
             i = NavMesh_NextNodeSlot(&currentSlot, i)) {
            struct NavMeshNode *node;
            NAV_MESH_GET_NODE(node, nodes, destinationRegion, i);
            NAV_MESH_GATHER_LINK_MEMBERS(node, regions, to, destinationRegions, destinationCount, j,
                                         t);
        }
    }

    {
        u16 current;
        for (t = 0; t < 6; t++)
            originRegions[t] = 0xFF;
        originRegion = &regions[current = state->region];
        originRegions[0] = current;
    }
    originCount = 1;
    for (i = 0; i <= 11 && originRegion->nodes[i] != 0xFF && originCount <= 5; i++) {
        struct NavMeshNode *node;
        NAV_MESH_GET_NODE(node, nodes, originRegion, i);
        NAV_MESH_COLLECT_NEIGHBORS(node, regions, from, originRegions, originCount, j, t);
    }
    for (i = 0; i < originCount; i++) {
        for (j = 0; j < destinationCount; j++) {
            if (originRegions[i] == destinationRegions[j]) {
                state->waypoint = state->destination = *to;
                state->origin = *from;
                state->status = 6;
                return 1;
            }
        }
    }

    shortest = 0xFFFF;
    waypointNode = 0xFF;
    targetNode = 0xFF;
    for (i = 0; i < originCount; i++) {
        originRegion = &regions[originRegions[i]];
        for (j = 0; j < destinationCount; j++) {
            destinationRegion = &regions[destinationRegions[j]];
            for (u = 0; u <= 11 && destinationRegion->nodes[u] != 0xFF; u++) {
                for (v = 0; v <= 11 && originRegion->nodes[v] != 0xFF; v++) {
                    u32 d = PairTable_Lookup(dist, mesh->nodeCount, destinationRegion->nodes[u],
                                             originRegion->nodes[v]);
                    if (d < shortest) {
                        shortest = d;
                        waypointNode = originRegion->nodes[v];
                        targetNode = destinationRegion->nodes[u];
                    } else if (d == shortest && shortest != 0xFFFF) {
                        s32 dx, dz, candidateDistance, previousDistance;
                        dx = from->x - nodes[originRegion->nodes[v]].x;
                        dz = from->z - nodes[originRegion->nodes[v]].z;
                        candidateDistance = dx * dx + dz * dz;
                        dx = from->x - nodes[waypointNode].x;
                        dz = from->z - nodes[waypointNode].z;
                        previousDistance = dx * dx + dz * dz;
                        if (candidateDistance < previousDistance) {
                            waypointNode = originRegion->nodes[v];
                            targetNode = destinationRegion->nodes[u];
                        }
                    }
                }
            }
        }
    }

    if (shortest <= 0xFFFE) {
        state->targetNode = targetNode;
        state->destination = *to;
        state->origin = *from;
        state->waypointNode = waypointNode;
        state->waypoint.x = nodes[waypointNode].x;
        state->waypoint.y = 0;
        state->waypoint.z = nodes[waypointNode].z;
        state->status = 3;
        return 1;
    }
    state->status = 0;
    return 0;
}
