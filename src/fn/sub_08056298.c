// MATCHING_CONCESSION: register-pinning
#include "terrain_edge_marker.h"
static inline u32 TerrainMarker_GetCellIndex(union TerrainMarkerPosition *position)
{
    s32 x = position->world.x >> 8;
    s32 z = position->world.z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline u32 TerrainMarker_GetInnerCellIndex(union TerrainMarkerPosition *position,
                                                  struct EngineMap **mapSlot)
{
    register u32 coordinate asm("r0") = (u16)position->world.x;
    register s32 x asm("r4");
    s32 z;
    coordinate <<= 16;
    x = (s32)coordinate >> 24;
    z = position->world.z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return (*mapSlot)->rows[z] + x;
}
static inline u32 TerrainMarker_GetCachedCellIndex(union TerrainMarkerPosition *position,
                                                   struct EngineMap **mapSlot)
{
    s32 x = position->world.x >> 8;
    s32 z = position->world.z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return (*mapSlot)->rows[z] + x;
}
static inline struct SceneCell *TerrainMarker_GetCell(u32 index, struct EngineMap **mapSlot)
{
    register u8 *cells asm("r1") = (*mapSlot)->cells;
    if (!cells)
        return 0;
    {
        u32 offset = index * 4 + 12;
        return (struct SceneCell *)(cells + offset);
    }
}
static inline struct SceneCell *TerrainMarker_GetOutsideCell(u32 index, struct EngineMap **input)
{
    register struct EngineMap **mapSlot asm("r4") = input;
    u8 *cells;
    asm("" : "+r"(mapSlot));
    cells = (*mapSlot)->cells;
    if (!cells)
        return 0;
    {
        u32 offset = index * 4 + 12;
        return (struct SceneCell *)(cells + offset);
    }
}
s32 TerrainEdgeMarker_FindTerrainDirection(struct TerrainMarkerRegion *input)
{
    register struct TerrainMarkerRegion *region asm("r12") = input;
    union TerrainMarkerPosition inside, outside;
    u32 insideIndex, outsideIndex;
    register struct SceneCell *insideCell asm("r2");
    struct SceneCell *outsideCell;
    struct EngineMap **mapSlot;
    /* Probe +X, then +Z, -X, and -Z, stopping at the first connection. */
    inside.packed.x = input->maxX - 16;
    {
        struct TerrainMarkerRegion *view = region;
        s32 maximum = view->maxZ;
        s32 minimum;
        minimum = view->minZ;
        inside.packed.z = (maximum + minimum) >> 1;
    }
    outside = inside;
    outside.packed.x += 32;
    insideIndex = TerrainMarker_GetCellIndex(&inside);
    outsideIndex = TerrainMarker_GetCellIndex(&outside);
    if (insideIndex == outsideIndex)
        return 0;
    {
        struct EngineMap **base = &gTerrainMap;
        register u8 *cells asm("r2") = (*base)->cells;
        mapSlot = base;
        asm("" : "+r"(mapSlot));
        if (!cells)
            insideCell = 0;
        else {
            u32 offset = insideIndex * 4 + 12;
            insideCell = (struct SceneCell *)(cells + offset);
        }
    }
    outsideCell = TerrainMarker_GetOutsideCell(outsideIndex, mapSlot);
    {
        register u32 raw asm("r0") = insideCell->height;
        register u32 mask asm("r4") = 15;
        register u32 fromKind asm("r2") = raw >> 4;
        register u32 sourceHeight asm("r5") = raw;
        u32 left, right;
        switch (fromKind) {
            case 0: {
                u32 rawOther = outsideCell->height;
                u32 otherKind = (rawOther >> 4) & mask;
                u32 other = rawOther;
                if (otherKind == 0) {
                    register u32 heightMask asm("r0") = 15;
                    left = heightMask;
                    left &= sourceHeight;
                    heightMask &= other;
                    if (left == heightMask) {
                        u32 flags = outsideCell->flags;
                        u32 flagMask = 1;
                        if (!(flags & flagMask))
                            return 0;
                    }
                }
                if ((other >> 4) == 2) {
                    right = 15;
                    left = right;
                    left &= sourceHeight;
                    right &= other;
                    goto compare_0;
                }
                break;
            }
            case 2: {
                u32 other = outsideCell->height;
                u32 otherKind = (other >> 4) & mask;
                if (otherKind == 0 || otherKind == 2) {
                    right = 15;
                    left = right;
                    left &= sourceHeight;
                    right &= other;
                    right++;
                compare_0:
                    if (left == right) {
                        u32 flags = outsideCell->flags;
                        u32 flagMask = 1;
                        if (!(flags & flagMask))
                            return 0;
                    }
                }
                break;
            }
            case 1:
                break;
        }
    }
    {
        struct TerrainMarkerRegion *view = region;
        inside.world.x = (view->maxX + view->minX) >> 1;
    }
    {
        register struct TerrainMarkerRegion *view asm("r4") = region;
        inside.world.z = view->maxZ - 16;
    }
    outside = inside;
    {
        union TerrainMarkerPosition *view = &outside;
        u32 coordinate;
        asm("" : "+r"(view));
        coordinate = (u16)view->world.z;
        coordinate += 32;
        view->world.z = coordinate;
    }
    insideIndex = TerrainMarker_GetInnerCellIndex(&inside, mapSlot);
    outsideIndex = TerrainMarker_GetCachedCellIndex(&outside, mapSlot);
    if (insideIndex == outsideIndex)
        return 1;
    insideCell = TerrainMarker_GetCell(insideIndex, mapSlot);
    outsideCell = TerrainMarker_GetOutsideCell(outsideIndex, mapSlot);
    {
        register u32 raw asm("r0") = insideCell->height;
        register u32 mask asm("r3") = 15;
        u32 fromKind = raw >> 4;
        u32 sourceHeight = raw;
        switch (fromKind) {
            case 0: {
                u32 rawOther = outsideCell->height;
                u32 otherKind = (rawOther >> 4) & mask;
                u32 other = rawOther;
                if (otherKind == 0) {
                    register u32 heightMask asm("r0") = 15;
                    register u32 left asm("r1") = heightMask;
                    left &= sourceHeight;
                    heightMask &= other;
                    if (left == heightMask) {
                        u32 flags = outsideCell->flags;
                        u32 flagMask = 1;
                        if (!(flags & flagMask))
                            return 1;
                    }
                }
                {
                    register u32 slopeKind asm("r4") = other >> 4;
                    if (slopeKind == 1) {
                        u32 heightMask = 15;
                        u32 left = heightMask;
                        left &= sourceHeight;
                        heightMask &= other;
                        asm("" : "+r"(left), "+r"(heightMask));
                        if (left == heightMask) {
                            u32 flags = outsideCell->flags;
                            if (!(flags & slopeKind))
                                return 1;
                        }
                    }
                }
                break;
            }
            case 1: {
                u32 other = outsideCell->height;
                u32 otherKind = (other >> 4) & mask;
                if (otherKind != 0) {
                    if (otherKind != 1)
                        break;
                }
                {
                    u32 heightMask = 15;
                    u32 left = heightMask;
                    left &= sourceHeight;
                    heightMask &= other;
                    heightMask++;
                    asm("" : "+r"(left), "+r"(heightMask));
                    if (left == heightMask) {
                        u32 flags = outsideCell->flags;
                        if (!(flags & fromKind))
                            return 1;
                    }
                }
                break;
            }
            case 2:
                break;
        }
    }
    {
        register struct TerrainMarkerRegion *view asm("r1") = region;
        inside.world.x = view->minX + 16;
    }
    {
        register struct TerrainMarkerRegion *view asm("r2") = region;
        s32 maximum;
        register u32 scratch asm("r3");
        s32 minimum;
        maximum = view->maxZ;
        /* Define an allocation-only scratch before spanning the signed load. */
        asm("" : "=r"(scratch) : "r"(maximum));
        minimum = view->minZ;
        asm("" : : "r"(scratch), "r"(minimum));
        inside.world.z = (maximum + minimum) >> 1;
    }
    outside = inside;
    outside.world.x -= 32;
    insideIndex = TerrainMarker_GetInnerCellIndex(&inside, mapSlot);
    outsideIndex = TerrainMarker_GetCachedCellIndex(&outside, mapSlot);
    if (insideIndex == outsideIndex)
        return 2;
    insideCell = TerrainMarker_GetCell(insideIndex, mapSlot);
    outsideCell = TerrainMarker_GetOutsideCell(outsideIndex, mapSlot);
    {
        register u32 raw asm("r0") = insideCell->height;
        register u32 mask asm("r6") = 15;
        register u32 fromKind asm("r2") = raw >> 4;
        register u32 sourceHeight asm("r5") = raw;
        u32 left, right;
        switch (fromKind) {
            case 0: {
                u32 rawOther = outsideCell->height;
                u32 otherKind = (rawOther >> 4) & mask;
                u32 other = rawOther;
                if (otherKind == 0) {
                    register u32 heightMask asm("r0") = 15;
                    u32 flatLeft = heightMask;
                    flatLeft &= sourceHeight;
                    heightMask &= other;
                    if (flatLeft == heightMask) {
                        u32 flags = outsideCell->flags;
                        u32 flagMask = 1;
                        if (!(flags & flagMask))
                            return 2;
                    }
                }
                {
                    u32 slopeKind = other >> 4;
                    if (slopeKind == 2) {
                        right = 15;
                        left = right;
                        left &= sourceHeight;
                        left++;
                        right &= other;
                        goto compare_2;
                    }
                }
                break;
            }
            case 2: {
                register u32 other asm("r4") = outsideCell->height;
                u32 otherKind = (other >> 4) & mask;
                if (otherKind == 0) {
                    register u32 heightMask asm("r0") = 15;
                    u32 flatLeft = heightMask;
                    flatLeft &= sourceHeight;
                    heightMask &= other;
                    if (flatLeft == heightMask)
                        goto clear_2;
                }
                if (otherKind == 2) {
                    right = 15;
                    left = right;
                    left &= sourceHeight;
                    left++;
                    right &= other;
                compare_2:
                    if (left == right) {
                    clear_2: {
                        u32 flags = outsideCell->flags;
                        u32 flagMask = 1;
                        if (!(flags & flagMask))
                            return 2;
                    }
                    }
                }
                break;
            }
            case 1:
                break;
        }
    }
    {
        struct TerrainMarkerRegion *view = region;
        inside.world.x = (view->maxX + view->minX) >> 1;
    }
    {
        register struct TerrainMarkerRegion *view asm("r4") = region;
        inside.world.z = view->minZ + 16;
    }
    outside = inside;
    {
        union TerrainMarkerPosition *view = &outside;
        u32 coordinate;
        asm("" : "+r"(view));
        coordinate = (u16)view->world.z;
        coordinate -= 32;
        view->world.z = coordinate;
    }
    insideIndex = TerrainMarker_GetInnerCellIndex(&inside, mapSlot);
    outsideIndex = TerrainMarker_GetCachedCellIndex(&outside, mapSlot);
    if (insideIndex == outsideIndex)
        return 3;
    insideCell = TerrainMarker_GetCell(insideIndex, mapSlot);
    outsideCell = TerrainMarker_GetOutsideCell(outsideIndex, mapSlot);
    {
        u32 raw = insideCell->height;
        register u32 mask asm("r6") = 15;
        u32 fromKind = raw >> 4;
        u32 sourceHeight = raw;
        u32 left, right;
        switch (fromKind) {
            case 0: {
                u32 rawOther = outsideCell->height;
                u32 otherKind = (rawOther >> 4) & mask;
                u32 other = rawOther;
                if (otherKind == 0) {
                    u32 heightMask = 15;
                    u32 flatLeft = heightMask;
                    flatLeft &= raw;
                    heightMask &= other;
                    asm("" : "+r"(flatLeft), "+r"(heightMask));
                    if (flatLeft == heightMask) {
                        u32 flags = outsideCell->flags;
                        u32 flagMask = 1;
                        if (!(flags & flagMask))
                            return 3;
                    }
                }
                {
                    register u32 slopeKind asm("r4") = other >> 4;
                    if (slopeKind == 1) {
                        right = 15;
                        left = right;
                        left &= sourceHeight;
                        left++;
                        right &= other;
                        if (left == right) {
                            u32 flags = outsideCell->flags;
                            if (!(flags & slopeKind))
                                return 3;
                        }
                    }
                }
                break;
            }
            case 1: {
                register u32 other asm("r4") = outsideCell->height;
                u32 otherKind = (other >> 4) & mask;
                if (otherKind == 0) {
                    register u32 heightMask asm("r0") = 15;
                    register u32 flatLeft asm("r1") = heightMask;
                    flatLeft &= raw;
                    heightMask &= other;
                    if (flatLeft == heightMask)
                        goto clear_3;
                }
                if (otherKind == 1) {
                    right = 15;
                    left = right;
                    left &= raw;
                    left++;
                    right &= other;
                    if (left == right) {
                    clear_3: {
                        u32 flags = outsideCell->flags;
                        u32 flagMask = 1;
                        if (!(flags & flagMask))
                            return 3;
                    }
                    }
                }
                break;
            }
            case 2:
                break;
        }
    }
    return -1;
}
