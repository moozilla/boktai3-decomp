#include "global.h"
#include "engine.h"
#include "nav_mesh.h"
#include "engine_globals.h"
#include "nav_steering.h"

void *ScriptProc_Start(u16, u16);

/* Install the requested navigation archive or clear the map slot if the resource is missing. */

u32 NavMesh_LoadResource(u16 id)
{
    void *navigation = ScriptProc_Start(0xF63B, id);
    if (navigation)
        gTerrainMap->navigation = navigation;
    else
        gTerrainMap->navigation = NULL;
    return 0;
}

void Terrain_SetNavMeshes(void *v)
{
    gTerrainMap->navigation = v;
}

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

u16 PairTable_Lookup(u16 *table, u16 stride, s32 a, s32 b)
{
    if (a == b)
        return 0;
    if (a > b)
        return table[b * stride + a - ((b + 2) * (b + 1) >> 1)];
    return table[a * stride + b - ((a + 2) * (a + 1) >> 1)];
}

u8 *TerrainModifiers_FindCellOverride(s32, s32);

u32 Nav_IsPositionInMesh(struct NavMeshQuery *q, void *unused, struct ContactPosition *pos)
{
    struct NavMeshResource *resource = gTerrainMap->navigation;
    u32 *new_var;
    s32 x = (s16)pos->x >> 8;
    s32 z = (s16)pos->z >> 8;
    s32 cell;
    u8 *props;
    u32 mask, flags;
    s32 meshIndex;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        cell = 0;
    else
        cell = gTerrainMap->rows[z] + x;
    props = TerrainModifiers_FindCellOverride(cell, 1);
    if (props)
        props += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = cell * 4 + 12;
        props = map->cells + offset;
    }
    flags = *(u16 *)(props + 2);
    mask = 2;
    if (flags & mask)
        return 0;
    for (meshIndex = 0; meshIndex < resource->count; meshIndex++) {
        u32 sh = meshIndex << 2;
        struct NavMeshGraph *mesh =
            (void *)((u8 *)resource + *(u32 *)((u32)(new_var = resource->offsets) + sh));
        struct NavMeshRegion *regions = (void *)((u8 *)mesh + mesh->regionOffset);
        s32 i;
        for (i = 0; i < mesh->count; i++) {
            if (NavMesh_IsPointInsideRegion(regions, pos, i)) {
                if (meshIndex == q->mesh)
                    return 1;
                return 0;
            }
        }
    }
    return 0;
}

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
        NAV_MESH_GATHER_LINK_MEMBERS(node, regions, from, originRegions, originCount, j, t);
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

int Terrain_ApplyBlocked(u16 *out, void *arg, struct ContactPosition *pos)
{
    s32 x = (s16)pos->x >> 8;
    s32 z = (s16)pos->z >> 8;
    s32 cell;
    u8 *props;
    u32 mask, flags;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        cell = 0;
    else
        cell = gTerrainMap->rows[z] + x;
    props = TerrainModifiers_FindCellOverride(cell, 1);
    if (props)
        props += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = cell * 4 + 12;
        props = map->cells + offset;
    }
    flags = *(u16 *)(props + 2);
    mask = 2;
    if (flags & mask) {
        int z = 0;
        *out = z;
        return z;
    }
    return NavMesh_FindRoute(out, arg, pos);
}


/* NavMesh_ClearProbeFlags: Clears the navigation probe's flag word */


void NavMesh_ClearProbeFlags(u16 *p)
{
    *p = 0;
}

u32 NavMesh_ConsumeProbeEvent(u16 *p)
{
    u32 r;
    if ((*p & 2) == 0)
        r = 0;
    else {
        *p &= 0xFFFD;
        r = 1;
    }
    return r;
}

static inline struct NavMeshGraph *GetMesh(struct NavMeshResource *res, u32 idx)
{
    u32 sh = idx << 2;
    u32 *offsets;
    return (void *)((u8 *)res + *(u32 *)((u32)(offsets = res->offsets) + sh));
}

void NavMesh_UpdateContainingRegion(void *resource, void *query, void *position)
{
    struct NavMeshResource *res = resource;
    struct NavMeshQuery *q = query;
    struct ContactPosition *pos = position;
    struct NavMeshGraph *mesh = GetMesh(res, q->mesh);
    struct NavMeshRegion *regions = (void *)((u8 *)mesh + mesh->regionOffset);
    struct NavMeshNode *links;
    struct NavMeshRegion *region;
    s32 i, j;

    if (NavMesh_IsPointInsideRegion(regions, pos, q->region))
        return;
    links = (void *)((u8 *)mesh + mesh->nodeOffset);
    region = &regions[q->region];
    for (i = 0; i < 12 && region->nodes[i] != 0xff; i++) {
        struct NavMeshNode *link;
        link = &links[region->nodes[i]];
        for (j = 0; j < 6; j++) {
            u8 *base = link->regions;
            u8 *k = base + j;
            if (*k == 0xff)
                j = 6;
            else if (*k != q->region && NavMesh_IsPointInsideRegion(regions, pos, *k)) {
                q->region = *k;
                return;
            }
        }
    }
    for (i = 0; i < mesh->count; i++) {
        if (i != q->region && NavMesh_IsPointInsideRegion(regions, pos, i)) {
            q->region = i;
            return;
        }
    }
    for (i = 0; i < res->count; i++) {
        if (i == q->mesh)
            continue;
        mesh = GetMesh(res, i);
        regions = (void *)((u8 *)mesh + mesh->regionOffset);
        for (j = 0; j < mesh->count; j++) {
            if (NavMesh_IsPointInsideRegion(regions, pos, j)) {
                q->mesh = i;
                q->region = j;
                return;
            }
        }
    }
}

/* NavMesh_StepTowardTarget: once the probe reaches its current waypoint node,
 * picks the neighbouring node with the smallest pair-table distance to the
 * target node and makes it the next waypoint (tentative). */

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

void NavMesh_StepTowardTarget(void *meshData, void *probeData, void *position)
{
    u8 *mesh = meshData;
    struct NavProbe *probe = probeData;
    s16 *pos = position;
    u32 sh = probe->region << 2;
    u32 *ov;
    struct NavMeshGraph *region =
        (struct NavMeshGraph *)(mesh + *(u32 *)((u32)(ov = (u32 *)(mesh + 4)) + sh));
    struct NavMeshRegion *edges = (struct NavMeshRegion *)((u8 *)region + region->regionOffset);
    struct NavMeshNode *nodes = (struct NavMeshNode *)((u8 *)region + region->nodeOffset);
    u16 *dist = (u16 *)((u8 *)region + region->distanceOffset);
    struct NavMeshNode *node = &nodes[probe->node];
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
            for (i = 0; i < 6 && node->regions[i] != 0xFF; i++) {
                struct NavMeshRegion *e = &edges[node->regions[i]];
                for (j = 0; j < 12 && e->nodes[j] != 0xFF; j++) {
                    if (e->nodes[j] != bestIdx) {
                        u16 d =
                            PairTable_Lookup(dist, region->nodeCount, probe->target, e->nodes[j]);
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

void NavMesh_UpdateProbe(struct NavSteeringProbe *probe, struct NavSteeringPosition *position)
{
    void *navigation = gTerrainMap->navigation;
    NavMesh_UpdateContainingRegion(navigation, probe, position);
    if (probe->flags & 1)
        NavMesh_StepTowardTarget(navigation, probe, position);
}

extern const s16 gUnk_086149C4[];

s32 sub_0821BF5C(s16 *, s16 *, s32);

static inline s32 Mul12(s32 a, s32 b)
{
    s32 t = a * b;
    if (t >= 0)
        return t >> 12;
    return -(-t >> 12);
}

s32 Actor_MoveAlongAngle(struct NavSteeringPosition *pos, u32 angle, s32 dist)
{
    struct NavSteeringPosition d;
    struct NavSteeringPosition *p = &d;
    struct NavSteeringPosition *q;
    p->x = Mul12(dist, gUnk_086149C4[(angle + 64) & 255]);
    p->y = 0;
    p->z = Mul12(dist, gUnk_086149C4[angle & 255]);
    q = &d;
    d.x = d.x + pos->x;
    q->y = pos->y + q->y;
    q->z = q->z + pos->z;
    return sub_0821BF5C((s16 *)pos, (s16 *)&d, 4);
}

// Steers a nav probe: when the probe lies to the side of `angle` (relative to `target`),

// or `target` cannot move along `angle`, snaps to a 4-way direction and sets flag 2.

s32 NavMesh_SteerCardinal(struct NavSteeringProbe *probe, struct NavSteeringPosition *target, s32 angle,
                 s32 dist)
{
    struct NavSteeringPosition d;
    s32 a, diff, ret;

    NavMesh_UpdateProbe(probe, target);
    probe->flags &= ~2;
    NavSteering_Subtract(&d, &probe->pos, target);
    a = Math_Atan2(d.x, d.z);
    diff = (a - angle) & 0xFF;
    if ((u32)(diff - 0x30) <= 0xA0) {
        ret = a + 0x20;
        ret &= 0xC0;
        probe->flags |= 2;
        return ret;
    }
    if (Actor_MoveAlongAngle(target, angle, dist) == 0) {
        if (diff <= 0x3F)
            a += 0x40;
        a &= 0xC0;
        NAV_STEERING_MARK_BLOCKED(probe);
        return a;
    }
    return angle;
}

// 8-direction variant of NavMesh_SteerCardinal: steers a nav probe; when the probe lies to the side of `angle`

// (relative to `target`), or `target` cannot move along `angle`, snaps to an 8-way direction and sets flag 2.

s32 NavMesh_SteerOctant(struct NavSteeringProbe *probe, struct NavSteeringPosition *target, s32 angle,
                 s32 dist)
{
    struct NavSteeringPosition d;
    s32 a, diff, ret;

    NavMesh_UpdateProbe(probe, target);
    probe->flags &= ~2;
    NavSteering_Subtract(&d, &probe->pos, target);
    a = Math_Atan2(d.x, d.z);
    diff = (a - angle) & 0xFF;
    if ((u32)(diff - 0x18) <= 0xD0) {
        ret = a + 0x10;
        ret &= 0xE0;
        probe->flags |= 2;
        return ret;
    }
    if (Actor_MoveAlongAngle(target, angle, dist) == 0) {
        if (diff <= 0x1F)
            a += 0x20;
        a &= 0xE0;
        NAV_STEERING_MARK_BLOCKED(probe);
        return a;
    }
    return angle;
}

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

struct NavHdr {
    u16 count;
    u16 pad;
    s32 offsets[1];
};

struct NavEntry {
    u16 count;
    u16 pad;
    s32 f4;
    s32 regionOffset;
};

struct Vec2 {
    s32 x, y;
};

struct Probe {
    u16 f0;
    u16 f2;
    u16 region;
    u16 index;
    s32 f8;
    struct Vec2 pos0;
    struct Vec2 pos1;
    struct Vec2 pos2;
};

struct World {
    u8 pad[0x14];
    struct NavHdr *nav;
};

// Declared non-void without a return statement: r0 is left unset.

u32 ProximityVolume_TranslateBounds(struct Probe *out, struct Vec2 *pos)
{
    struct NavHdr *hdr = ((struct World *)gTerrainMap)->nav;
    s32 i, j;

    out->region = 0xff;
    out->index = 0xff;
    for (i = 0; i < hdr->count; i++) {
        s32 off = i * 4;
        s32 *offs = hdr->offsets;
        struct NavEntry *e = (struct NavEntry *)((u8 *)hdr + *(s32 *)((u8 *)offs + off));
        u8 *region = (u8 *)e + e->regionOffset;
        for (j = 0; j < e->count; j++) {
            if (NavMesh_IsPointInsideRegion((struct NavMeshRegion *)region,
                                            (struct ContactPosition *)pos, j)) {
                out->region = i;
                out->index = j;
            }
        }
    }
    out->f0 = 0;
    out->pos0 = *pos;
    out->pos2 = *pos;
    out->pos1 = *pos;
}
