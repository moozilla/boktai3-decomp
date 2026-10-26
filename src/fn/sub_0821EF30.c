#include "terrain_cell_probe.h"

// The random table (gUnk_0203B400) is addressed by its numeric address here; the
// symbol form changes reload order and register choice in the loop.

u8 *Path_GetDefinition(u32 id);
s32 Path_GetPointData(u8 *def);
void Path_GetPointPosition(struct Pos16 *out, s32 data, u32 index);
s32 Mod(s32, s32);

struct PosS16 {
    s16 x, y, z, w;
};

static inline u8 PosInBox(struct PosS16 *p, struct PosS16 *lo, struct PosS16 *hi)
{
    if (!(p->x >= lo->x && p->x <= hi->x))
        return FALSE;
    if (p->z >= lo->z && p->z <= hi->z)
        return TRUE;
    return FALSE;
}

s32 Path_SnapToRandomPointInBox(struct Pos16 *out, struct Pos16 *lo, struct Pos16 *hi, u32 pathId)
{
    s32 best = -1;
    u8 *def = Path_GetDefinition(pathId);
    s32 data;
    s32 start, i;
    struct Pos16 pos;
    if (!def || !def[0]) {
        out->x = 0;
        out->y = 0;
        out->z = 0;
        return best;
    }
    data = Path_GetPointData(def);
    if (!data) {
        out->x = data;
        out->y = data;
        out->z = data;
        return best;
    }
    {
        u32 r = (gRandomIndex + 1) & 0x3FF;
        gRandomIndex = r;
        start = Mod(*(const u16 *)((u8 *)0x0203B400 + r * 2), def[0]);
    }
    Path_GetPointPosition(&pos, data, 0);
    out->x = pos.x;
    out->z = pos.z;
    for (i = start; i < start + def[0]; i++) {
        s32 idx = Mod(i, def[0]);
        Path_GetPointPosition(&pos, data, idx);
        if (PosInBox((struct PosS16 *)&pos, (struct PosS16 *)lo, (struct PosS16 *)hi)) {
            best = idx;
            out->x = pos.x;
            out->z = pos.z;
        }
    }
    Pos16_SnapToTerrain(out);
    return best;
}
