#include "global.h"

// confidence=low: walks the terrain grid from the actor's cell in up to four directions and writes its new position.

// confidence=low: walks the terrain grid from the actor's cell in up to four directions and writes its new position.

#include "engine.h"
#include "contact_actor.h"
#include "animated_hazard.h"
// confidence=low: walks the terrain grid from the actor's cell in up to four directions and writes its new position.

// confidence=low: walks the terrain grid from the actor's cell in up to four directions and writes its new position.

// confidence=low: walks the terrain grid from the actor's cell in up to four directions and writes its new position.
static inline u8 *F0807E334_Cell(s32 idx)
{
    u8 *ov = TerrainModifiers_FindCellOverride(idx, 1);
    if (ov != NULL)
        return ov + 4;
    return gUnk_030052F4->cells + 12 + idx * 4;
}
static inline s32 F0807E334_Kind(s32 idx)
{
    return F0807E334_Cell(idx)[0] & 0xf;
}
static inline s32 F0807E334_Open(s32 idx, s32 kind)
{
    if (F0807E334_Kind(idx) != kind)
        return 0;
    if (*(u16 *)(F0807E334_Cell(idx) + 2) & 0x43)
        return 0;
    return 1;
}
#ifdef NONMATCHING
// fuzzy: 29.7%
// scope shape 345 (differences once low registers are set aside)
void sub_0807E334(void *arg0)
{
    u8 *self = arg0;
    u8 *f3d4 = self + 0x3d4;
    s32 mode = ((((s32)self[0x59] + 0x20) >> 6) & 3) + 2 & 3;
    s32 sb = 0;
    s32 cell;
    s32 newCell;
    s32 kind;
    s32 width;
    s32 total;
    s32 idx;
    s32 x;
    struct ContactPosition pos;
    s32 clear;

    do {
        sb++;
        cell = *(u16 *)(arg0 + 0x2f4);
        kind = F0807E334_Kind(cell);
        width = *(s16 *)(gUnk_030052F4->cells + 4);
        total = width * *(s16 *)(gUnk_030052F4->cells + 6);
        newCell = cell;
        if (mode == 1) {
            idx = cell + width;
            while (total > idx) {
                if (!F0807E334_Open(idx, kind)) {
                    newCell = idx - width;
                    break;
                }
                idx += width;
            }
        } else if (mode > 1) {
            if (mode == 2) {
                idx = cell - 1;
                x = Mod(idx, width);
                while (x >= 0) {
                    if (!F0807E334_Open(idx, kind)) {
                        newCell = idx + 1;
                        break;
                    }
                    idx--;
                    x--;
                }
            } else if (mode == 3) {
                idx = cell - width;
                while (idx >= 0) {
                    if (!F0807E334_Open(idx, kind)) {
                        newCell = idx + width;
                        break;
                    }
                    idx -= width;
                }
            }
        } else if (mode == 0) {
            idx = cell + 1;
            x = Mod(idx, width);
            while (x < width) {
                if (!F0807E334_Open(idx, kind)) {
                    newCell = idx - 1;
                    break;
                }
                idx++;
                x++;
            }
        }
        mode = (mode + 1) & 3;
    } while (newCell == cell && sb <= 3);

    pos.x = (Mod(newCell, width) << 8) + 0x80;
    pos.y = F0807E334_Kind(newCell) << 8;
    pos.z = (Div(newCell, width) << 8) + 0x80;

    clear = 1;
    if (Nav_IsPositionInMesh(*(void **)(self + 0x5d0), (struct ContactPosition *)(self + 0x5c),
                             &pos)) {
        s32 r = Terrain_ApplyBlocked(*(void **)(self + 0x5d0),
                                     (struct ContactPosition *)(self + 0x5c), &pos);
        if (r == 2)
            clear = 0;
        else if (r != 0) {
            Memory_Clear(self + 0x64, 8);
            clear = 0;
        }
    }
    if (clear)
        *(u16 *)(self + 0x2d4) &= 0xFDFF;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0807E334);
#endif
