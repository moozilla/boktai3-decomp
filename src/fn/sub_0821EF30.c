#include "global.h"
#include "engine.h"
struct Position {
    s16 x, y, z, pad;
};
u8 *Path_GetDefinition(u32);
void *Path_GetPointData(u8 *);
void Path_GetPointPosition(struct Position *, void *, u32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
static inline s32 CellIndex(struct Position *p)
{
    s32 x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}
static inline u8 *CellData(s32 i)
{
    u8 *p = TerrainModifiers_FindCellOverride(i, 1);
    if (p)
        return p + 4;
    return gUnk_030052F4->cells + (i * 4 + 12);
}
static inline s32 Height(struct Position *p)
{
    u8 *data = CellData(CellIndex(p));
    s32 shape = *data >> 4, h = (*data & 15) << 8;
    switch (shape) {
        case 1:
            h -= (u8)p->z;
            break;
        case 2:
            h -= (u8)p->x;
            break;
    }
    return h;
}
extern u32 gUnk_03005308;

s32 Mod(s32, s32);
static inline u8 Within(struct Position *p, struct Position *low, struct Position *high)
{
    if (p->x >= low->x && p->x <= high->x && p->z >= low->z && p->z <= high->z)
        return 1;
    return 0;
}
// confidence=low: scans path points in randomized order and retains the last inside bounds.
#define Path_FindPointWithinBounds sub_0821EF30
#ifdef NONMATCHING
// fuzzy: 78.5%
// Remaining differences are register allocation in bounds tests and RNG lookup.
s32 Path_FindPointWithinBounds(struct Position *out, struct Position *low, struct Position *high,
                               u32 id)
{
    struct Position point;
    s32 chosen = -1;
    u8 *def = Path_GetDefinition(id);
    void *data;
    s32 start, i;
    if (!def || !*def) {
        out->x = 0;
        out->y = 0;
        out->z = 0;
        return chosen;
    }
    data = Path_GetPointData(def);
    if (!data) {
        out->x = 0;
        out->y = 0;
        out->z = 0;
        return chosen;
    }
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    start = Mod(*(const u16 *)((gUnk_03005308 << 1) + (u32)gUnk_0203B400), *def);
    Path_GetPointPosition(&point, data, 0);
    out->x = point.x;
    out->z = point.z;
    for (i = start; i < start + *def; i++) {
        s32 index = Mod(i, *def);
        Path_GetPointPosition(&point, data, index);
        if (Within(&point, low, high)) {
            chosen = index;
            out->x = point.x;
            out->z = point.z;
        }
    }
    out->y = Height(out);
    return chosen;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0821EF30);
#endif
