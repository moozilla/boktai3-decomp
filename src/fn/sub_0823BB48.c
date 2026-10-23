#include "global.h"
#include "terrain_cell_probe.h"
static inline u32 Height(struct Pos16 *p)
{
    u8 *props = cellProps(cellIndexAt(p));
    u32 kind = *props >> 4;
    u32 h = (*props & 15) << 8;
    if (kind == 1)
        goto slopeZ;
    if (kind == 2)
        goto slopeX;
    goto done;
slopeZ:
    h -= *(u8 *)&p->z;
    goto done;
slopeX:
    h -= *(u8 *)&p->x;
done:
    return h;
}
static inline u32 CellFlag(u8 *props)
{
    u16 *field = (u16 *)(props + 2);
    u32 value = *field;
    return value;
}
// confidence=low: rejects a candidate position if terrain height changes or its cell blocks movement.
#define Player_IsTerrainStepBlocked sub_0823BB48
s32 Player_IsTerrainStepBlocked(u8 *p, struct Pos16 *position)
{
    if (Height(position) != *(s16 *)(p + 0x32))
        return 1;
    {
        u32 flags = CellFlag(cellProps(cellIndexAt(position)));
        flags &= 1;
        if (flags)
            return 1;
    }
    return 0;
}
