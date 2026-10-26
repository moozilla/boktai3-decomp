// MATCHING_CONCESSION: register-pinning
#include "scene_grid.h"
extern const s8 gUnk_08605BC8[];

/* Negative results encode a cell two steps beyond a blocked candidate. */
s32 SceneGrid_ScanRows(s32 columnInput, s32 rowInput, s32 blockedInput, s32 kindInput, s32 steps)
{
    register s32 column asm("r6") = columnInput;
    s32 row = rowInput;
    s32 blocked = blockedInput;
    register s32 kind asm("r5") = kindInput;
    register struct EngineMap **location asm("r10");
    s32 count;
    s32 cell;
    s32 width;
    asm("" : "+l"(steps));
    if (steps)
        goto haveSteps;
noPosition:
    return -255;
haveSteps:
    if (steps < 0) {
        s32 limit;
        s32 beyond;
        count = 1;
        limit = -(u32)steps;
        beyond = row + 1;
        {
            struct EngineMap **base = &gTerrainMap;
            location = base;
        }
        if (count <= limit) {
            register s32 previous asm("r0") = row - 1;
            if (previous >= 0) {
                const s8 *table = gUnk_08605BC8;
                previous *= 8;
                previous = column + previous;
                previous += (u32)table;
                if (*(const s8 *)previous == kind) {
                    s32 bound = limit;
                    register u32 address asm("r0");
                    u32 read;
                    address = row * 8 + column;
                    address += (u32)table;
                    asm("" : "+r"(address));
                    read = address;
                    read -= 8;
                    do {
                        read -= 8;
                        count++;
                    } while (count <= bound && row - count >= 0 && *(const s8 *)read == kind);
                }
            }
        }
        {
            s32 next = beyond;
            row = next - count;
        }
    } else {
        register s32 before asm("r3");
        count = 1;
        before = row - 1;
        location = &gTerrainMap;
        if (count <= steps) {
            register s32 next asm("r0") = row + 1;
            if (next <= 9) {
                const s8 *table = gUnk_08605BC8;
                next *= 8;
                next = column + next;
                next += (u32)table;
                if (*(const s8 *)next == kind) {
                    register u32 address asm("r0") = row * 8 + column;
                    u32 read;
                    address += (u32)table;
                    asm("" : "+r"(address));
                    read = address;
                    read += 8;
                    do {
                        read += 8;
                        count++;
                    } while (count <= steps && row + count <= 9 && *(const s8 *)read == kind);
                }
            }
        }
        row = before + count;
    }
    {
        s32 worldRow = row + 8;
        register struct EngineMap **map asm("r2");
        u8 *grid;
        map = location;
        grid = (*map)->cells;
        {
            u32 offset = 4;
            width = *(s16 *)(grid + offset);
        }
        {
            s32 product = worldRow;
            product *= width;
            product += 5;
            cell = column + product;
        }
    }
    if (cell == blocked) {
        if (steps < 0) {
            register s32 next asm("r1") = row - 2;
            if (next < 0)
                goto noPosition;
            {
                const s8 *table = gUnk_08605BC8;
                next *= 8;
                next = column + next;
                next += (u32)table;
                if (*(const s8 *)next != kind)
                    goto noPosition;
            }
            return width * 2 - cell;
        } else {
            register s32 next asm("r1") = row + 2;
            if (next > 9)
                goto noPosition;
            {
                const s8 *table = gUnk_08605BC8;
                next *= 8;
                next = column + next;
                next += (u32)table;
                if (*(const s8 *)next != kind)
                    goto noPosition;
            }
            {
                s32 doubled = width * 2;
                return -(cell + doubled);
            }
        }
    }
    return cell;
}
