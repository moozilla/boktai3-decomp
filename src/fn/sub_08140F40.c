#include "global.h"
#include "player.h"
#include "engine.h"
#include "ram.h"

struct Terrain {
    u8 pad[4];
    u8 *cells;
};
u8 *TerrainModifiers_FindCellOverride(s32, s32);
static inline u8 *Cell(s32 i)
{
    u8 *p = TerrainModifiers_FindCellOverride(i, 1);
    if (p)
        return p + 4;
    return gTerrainMap->cells + 12 + i * 4;
}
static inline s32 Width(void)
{
    return *(s16 *)(gTerrainMap->cells + 4);
}
s32 Player_GetForceDirection(struct Player *p)
{
    s32 angle = ((gPlayerBackgroundMode + 0x70) & 255);
    s32 d;
    s32 offset;
    u32 h;
    /* Keep the signed quantization used by the original compiler. */
    d = angle >> 5;
    offset = 0;
    if (d & 1) {
        if (d == 1) {
            offset = Width();
        } else if (d == 3) {
            offset = -1;
        } else if (d == 5) {
            offset = -Width();
        } else if (d == 7) {
            offset = 1;
        }
        h = *Cell(p->recoveryTerrainCell) & 15;
        if (h < ((*Cell(p->recoveryTerrainCell + offset)) & 15)) {
            goto fail;
        }
    } else {
        s32 x;
        s32 z;
        if (d == 0) {
            offset = Width() + 1;
        } else if (d == 2) {
            offset = Width() - 1;
        } else if (d == 4) {
            offset = (-Width()) - 1;
        } else if (d == 6) {
            offset = (-Width()) + 1;
        }
        h = (*Cell(p->recoveryTerrainCell)) & 15;
        if (h < ((*Cell(p->recoveryTerrainCell + offset)) & 15)) {
            goto fail;
        }
        x = ((u16)p->position.x) & 255;
        z = ((u16)p->position.z) & 255;
        if (d == 0) {
            if (x > z) {
                offset = 1;
            } else {
                offset = Width();
            }
        } else if (d == 2) {
            if ((x + z) > 256) {
                offset = Width();
            } else {
                offset = -1;
            }
        } else if (d == 4) {
            if (x > z) {
                offset = -Width();
            } else {
                offset = -1;
            }
        } else if (d == 6) {
            if ((x + z) > 256) {
                offset = 1;
            } else {
                offset = -Width();
            }
        }
        h = (*Cell(p->recoveryTerrainCell)) & 15;
        if (h < ((*Cell(p->recoveryTerrainCell + offset)) & 15)) {
            goto fail;
        }
    }
    do {
        do {
            angle = d + 1;
            d = angle << 5;
            return d;
        } while (0);
    } while (0);
fail:
    return -1;
}
