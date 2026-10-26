#include "global.h"
#include "engine.h"
#include "player.h"
u8 *TerrainModifiers_FindCellOverride(s32, s32);
void Player_UpdateFacingSide(struct Player *);
void ActorSequence_Begin(void *, s32, s32);
extern const s16 gUnk_086149C4[];

struct FloorPoint {
    s16 x, y, z, pad;
};
struct PackedVec {
    u32 x : 16, y : 16, z : 16, pad : 16;
};
static inline s32 Scale(int x)
{
    if (x >= 0)
        return x >> 12;
    return -(-x >> 12);
}
static inline s32 cellIndex(struct FloorPoint *pos)
{
    s32 x = pos->x >> 8, z = pos->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline u32 groundHeight(struct FloorPoint *position)
{
    s32 index = cellIndex(position), height;
    u8 *cell = TerrainModifiers_FindCellOverride(index, 1);
    u32 kind;
    if (cell)
        cell += 4;
    else
        cell = gTerrainMap->cells + 12 + index * 4;
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

// confidence=low: compares nearby terrain height and input heading before reversing facing;
// Player_TryStepForward (sub_08242120) with a facing update and sequence restart at the end.
#define Player_CheckFacingTerrainClearance sub_08156DAC
s32 Player_CheckFacingTerrainClearance(struct Player *p)
{
    u32 zero = *((u8 *)p + 0x4E3);
    if (zero != 0)
        return 1;
    {
        u8 axis = p->coordinateAxis;
        u32 a = (axis + 7) & 7;
        u32 b = (axis + 9) & 7;
        s32 h = (s16)p->directionHistory[0];
        if (h == axis || h == a || h == b) {
            struct PackedVec pt;
            u32 ang = ((p->coordinateAxis + 1) & 7) << 5;
            u32 height;
            pt = *(struct PackedVec *)&p->position;
            pt.x += -Scale(gUnk_086149C4[(ang + 0x40) & 0xff] * 0x32);
            pt.z += -Scale(gUnk_086149C4[ang] * 0x32);
            height = groundHeight((struct FloorPoint *)&pt);
            if (height >= (u32)(p->position.y + 0xc8))
                return 1;
            p->facing = (p->coordinateAxis + 4) & 7;
        } else {
            p->facing = (axis + 4) & 7;
        }
    }
    Player_UpdateFacingSide(p);
    ActorSequence_Begin(p, 0, 0);
    return 0;
}
