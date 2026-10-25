#include "engine.h"
#include "contact_actor.h"

// confidence=low: links the point pair into the navigation graph; returns 0, 1 or 2 and records the result in the state.
struct S0821F2EC_Edge {
    u8 x0, z0, x1, z1;
    u8 nodes[12];
};

struct S0821F2EC_Node {
    u16 x, z;
    u16 unk4;
    u8 links[6];
};

struct S0821F2EC_Region {
    u16 count, stride;
    u32 unk4;
    u32 edgesOffset;
    u32 nodesOffset;
    u32 distOffset;
};

struct S0821F2EC_Table {
    u16 count, unk2;
    u32 offsets[1];
};

struct S0821F2EC_State {
    u16 status;
    u8 unk2[2];
    u16 region;
    u16 edge;
    u16 bestA;
    u16 bestB;
    struct ContactPosition a;
    struct ContactPosition b;
    struct ContactPosition c;
};

u32 NavMesh_IsPointInsideRegion(void *, const struct ContactPosition *, u32);
u16 PairTable_Lookup(u16 *table, u16 stride, s32 a, s32 b);

static inline s32 S0821F2EC_Dist2(const struct ContactPosition *from,
                                  const struct S0821F2EC_Node *node)
{
    s32 dx = from->x - node->x;
    s32 dz = from->z - node->z;
    return dx * dx + dz * dz;
}

#ifdef NONMATCHING
// fuzzy: 27.9%
// scope shape 366 (differences once low registers are set aside)
// confidence=low: links the point pair into the navigation graph; returns 0, 1 or 2 and records the result in the state.
u32 sub_0821F2EC(void *arg0, void *arg1, void *arg2)
{
    struct S0821F2EC_State *state = arg0;
    const struct ContactPosition *from = arg1;
    const struct ContactPosition *to = arg2;
    struct S0821F2EC_Table *table;
    struct S0821F2EC_Region *region = 0;
    struct S0821F2EC_Edge *edges = 0;
    struct S0821F2EC_Edge *edgeA;
    struct S0821F2EC_Edge *edgeB;
    struct S0821F2EC_Node *nodes;
    u16 *dist;
    u16 listA[6];
    u16 listB[6];
    s32 countA, countB;
    s32 foundRegion, foundEdge;
    s32 i, j, k, m, t, u, v;
    u32 bestDist, bestA, bestB;

    if (state->a.x == to->x && state->a.z == to->z && state->b.x == from->x &&
        state->b.z == from->z)
        return 2;
    state->a.x = to->x;
    state->a.z = to->z;

    table = gUnk_030052F4->navigation;
    foundRegion = 0xFF;
    foundEdge = 0xFF;
    for (i = 0; i < table->count; i++) {
        region = (void *)((u8 *)table + table->offsets[i]);
        edges = (void *)((u8 *)region + region->edgesOffset);
        for (k = 0; k < region->count; k++) {
            if (NavMesh_IsPointInsideRegion(edges, to, k)) {
                foundRegion = i;
                foundEdge = k;
            }
        }
        if (foundRegion == i)
            break;
    }
    if (foundEdge == 0xFF) {
        state->status = 0;
        return 0;
    }
    if (state->region != foundRegion) {
        state->status = 0;
        return 0;
    }
    if (state->edge == foundEdge) {
        state->a = *to;
        state->b = *from;
        state->c = *to;
        state->status = 6;
        return 1;
    }
    if (region == 0 || edges == 0)
        return 0;

    nodes = (void *)((u8 *)region + region->nodesOffset);
    dist = (void *)((u8 *)region + region->distOffset);
    for (t = 0; t < 6; t++) {
        listA[t] = 0xFF;
        listB[t] = 0xFF;
    }

    listA[0] = foundEdge;
    countA = 1;
    edgeA = &edges[foundEdge];
    for (j = 0; j <= 11 && edgeA->nodes[j] != 0xFF && countA <= 5; j++) {
        struct S0821F2EC_Node *node = &nodes[edgeA->nodes[j]];
        for (m = 0; m <= 5 && node->links[m] != 0xFF && countA <= 5; m++) {
            u8 link = node->links[m];
            if (link != listA[0] && NavMesh_IsPointInsideRegion(edges, to, link)) {
                s32 isNew = 1;
                for (t = 0; t < countA; t++) {
                    if (listA[t] == link)
                        isNew = 0;
                }
                if (isNew) {
                    listA[countA] = link;
                    countA++;
                }
            }
        }
    }

    listB[0] = state->edge;
    countB = 1;
    edgeB = &edges[state->edge];
    for (j = 0; j <= 11 && edgeB->nodes[j] != 0xFF && countB <= 5; j++) {
        struct S0821F2EC_Node *node = &nodes[edgeB->nodes[j]];
        for (m = 0; m <= 5 && node->links[m] != 0xFF && countB <= 5; m++) {
            u8 link = node->links[m];
            if (link != listB[0] && NavMesh_IsPointInsideRegion(edges, from, link)) {
                s32 isNew = 1;
                for (t = 0; t < countB; t++) {
                    if (listB[t] == link)
                        isNew = 0;
                }
                if (isNew) {
                    listB[countB] = link;
                    countB++;
                }
            }
        }
    }

    for (i = 0; i < countB; i++) {
        for (j = 0; j < countA; j++) {
            if (listB[i] == listA[j]) {
                state->a = *to;
                state->b = *from;
                state->c = *to;
                state->status = 6;
                return 1;
            }
        }
    }

    bestDist = 0xFFFF;
    bestA = 0xFF;
    bestB = 0xFF;
    for (i = 0; i < countB; i++) {
        edgeB = &edges[listB[i]];
        for (j = 0; j < countA; j++) {
            edgeA = &edges[listA[j]];
            for (u = 0; u <= 11 && edgeA->nodes[u] != 0xFF; u++) {
                for (v = 0; v <= 11 && edgeB->nodes[v] != 0xFF; v++) {
                    u32 nodeA = edgeA->nodes[u];
                    u32 nodeB = edgeB->nodes[v];
                    u32 d = PairTable_Lookup(dist, region->stride, nodeA, nodeB);
                    if (d < bestDist) {
                        bestDist = d;
                        bestB = nodeB;
                        bestA = nodeA;
                    } else if (d == bestDist && bestDist != 0xFFFF) {
                        if (S0821F2EC_Dist2(from, &nodes[nodeB]) <
                            S0821F2EC_Dist2(from, &nodes[bestB])) {
                            bestB = nodeB;
                            bestA = nodeA;
                        }
                    }
                }
            }
        }
    }

    if (bestDist <= 0xFFFE) {
        state->bestA = bestA;
        state->bestB = bestB;
        state->a = *to;
        state->b = *from;
        state->c = *to;
        state->c.x = nodes[bestB].x;
        state->c.y = 0;
        state->c.z = nodes[bestB].z;
        state->status = 3;
        return 1;
    }
    state->status = 0;
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0821F2EC);
#endif
