#include "terrain_cell_probe.h"
#include "contact_position.h"

s32 sub_0821D178(struct ContactPosition *out, struct ContactPosition *from,
                 struct ContactPosition *target, s32 previousStep);

static inline u32 cellHeightAt(struct ContactPosition *pos, s32 cell)
{
    u8 *props = cellProps(cell);
    u32 type = *props >> 4;
    u32 h = (*props & 0xf) << 8;
    switch (type) {
        case 1:
            h -= (u8)pos->z;
            break;
        case 2:
            h -= (u8)pos->x;
            break;
    }
    return h;
}

// Walks up to `steps` path steps from `start`; returns 1 when the walk finishes
// (sub_0821D178 returns 1) before the terrain rises `maxRise` above start->y, else 0.
s32 sub_0821D4F0(struct ContactPosition *start, struct ContactPosition *target, s32 steps,
                 s32 maxRise)
{
    struct ContactPosition cur, next;
    s32 i, cell, stepResult;

    if (gTerrainMap->cells == NULL)
        return 0;
    stepResult = 0;
    cur = *start;
    cell = cellIndexAt((struct Pos16 *)&cur);
    for (i = 0; i < steps; i++) {
        struct ContactPosition *nextPos = &next;
        stepResult = sub_0821D178(nextPos, &cur, target, stepResult);
        if (stepResult > 1)
            cell = cellIndexAt((struct Pos16 *)nextPos);
        if ((s32)(cellHeightAt(&next, cell) - start->y) >= maxRise)
            return 0;
        if (stepResult == 1)
            return 1;
        cur = next;
    }
    return 0;
}
