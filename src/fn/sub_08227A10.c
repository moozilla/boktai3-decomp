#include "global.h"
#include "engine.h"
u8 *sub_0821D64C(s32, s32);
void sub_0821AAD8(void *);
void sub_0821B4C8(void *, u32, s32);
struct FloorPoint {
    s16 x, y, z, pad;
};
union FloorPointWords {
    struct FloorPoint point;
    struct {
        u32 x : 16;
        u32 y : 16;
        u32 z : 16;
    } words;
};
static inline s32 cellIndex(struct FloorPoint *pos)
{
    s32 x = pos->x >> 8, z = pos->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}
static inline s32 groundHeight(struct FloorPoint *position)
{
    s32 index = cellIndex(position), height;
    u8 *cell = sub_0821D64C(index, 1);
    u32 kind;
    if (cell)
        cell += 4;
    else
        cell = gUnk_030052F4->cells + 12 + index * 4;
    kind = *cell >> 4;
    height = (*cell & 15) << 8;
    switch (kind) {
        case 1:
            height -= (u8)position->z;
            break;
        case 2:
            height -= (u8)position->x;
            break;
    }
    return height;
}
/* Script: floor height of the terrain cell at world (x, z), slope included;
   stores it into the result variable and returns it. */
s32 Terrain_GetFloorHeightFromScript(void)
{
    union FloorPointWords pos;
    s32 height;
    u32 result[2];
    pos.words.x = Script_GetValue();
    pos.words.y = 0;
    pos.words.z = Script_GetValue();
    height = groundHeight(&pos.point);
    sub_0821AAD8(result);
    sub_0821B4C8(result, 0, height);
    return height;
}
