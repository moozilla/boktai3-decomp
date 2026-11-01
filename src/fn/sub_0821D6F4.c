#include "global.h"
#include "engine.h"
#include "terrain_cell_probe.h"
// confidence=low: tests higher adjacent cells across the selected subcell corner.
struct Position {
    s16 x, y, z, pad;
};
struct Sample {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
// confidence=low: compares base heights at positive subcell corners.
s32 Terrain_HasHigherCorner(struct Position *p, s32 threshold)
{
    s32 x = (p->x & 0xff00) + 128;
    s32 y = p->y & 0xff00;
    s32 z = (p->z & 0xff00) + 128;
    s32 index = cellIndexAt((struct Pos16 *)p);
    u8 *data = cellProps(index);
    s32 height;
    struct Sample sample;
    u32 low;
    if (!data)
        return 0;
    height = *data & 15;
    low = 255;
    if ((u8)p->x < threshold) {
        low &= (u16)p->z;
        if ((s32)low < threshold)
            return 0;
        sample.x = x;
        sample.y = y;
        sample.z = z + 255;
        data = cellProps(cellIndexAt((struct Pos16 *)&sample));
    } else {
        low &= (u16)p->z;
        if ((s32)low >= threshold) {
            sample.x = x + 255;
            sample.y = y;
            sample.z = z;
            data = cellProps(cellIndexAt((struct Pos16 *)&sample));
            if (height < (*data & 15))
                return 1;
            sample.x = x;
            sample.y = y;
            sample.z = z + 255;
            data = cellProps(cellIndexAt((struct Pos16 *)&sample));
            if (height < (*data & 15))
                return 1;
            sample.x = x + 255;
            sample.y = y;
            sample.z = z + 255;
            data = cellProps(cellIndexAt((struct Pos16 *)&sample));
        } else {
            sample.x = x + 255;
            sample.y = y;
            sample.z = z;
            data = cellProps(cellIndexAt((struct Pos16 *)&sample));
        }
    }
    if (height >= (*data & 15))
        return 0;
    return 1;
}
