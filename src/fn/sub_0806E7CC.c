// MATCHING_CONCESSION: register-pinning
#include "scene_grid.h"
extern const s8 gUnk_08605BC8[];
#define SceneGrid_ScanColumns sub_0806E7CC

/* Negative results encode a cell two steps beyond a blocked candidate. */
s32 SceneGrid_ScanColumns(s32 columnInput,s32 rowInput,s32 blockedInput,s32 kindInput,s32 steps)
{
    s32 column = columnInput;
    s32 row = rowInput;
    s32 blocked = blockedInput;
    register s32 kind asm("r3") = kindInput;
    register s32 count asm("r2");
    s32 rowPlus;
    s32 cell;
    asm("" : "+l"(steps));
    if (steps) goto haveSteps;
noPosition:
    return -255;
haveSteps:
    if (steps < 0) {
        s32 limit;
        s32 beyond;
        count = 1;
        limit = -(u32)steps;
        beyond = column+1;
        rowPlus = 8+row;
        if (count <= limit) {
            s32 previous = column-1;
            if (previous >= 0) {
                const s8 *table = gUnk_08605BC8;
                register s32 offset asm("r0") = row*8;
                previous += offset;
                previous += (u32)table;
                if (*(const s8 *)previous == kind) {
                    u32 read;
                    offset = column+offset;
                    offset += (u32)table;
                    read = offset-1;
                    do {
                        read--;
                        count++;
                    } while (count <= limit && column-count >= 0 && *(const s8 *)read == kind);
                }
            }
        }
        {
            s32 next = beyond;
            column = next-count;
        }
    } else {
        register s32 before asm("ip");
        count = 1;
        before = column-1;
        rowPlus = 8+row;
        if (count <= steps) {
            s32 next = column+1;
            if (next <= 7) {
                register const s8 *table asm("r5") = gUnk_08605BC8;
                register s32 offset asm("r0") = row*8;
                next += offset;
                next += (u32)table;
                if (*(const s8 *)next == kind) {
                    u32 read;
                    offset = column+offset;
                    offset += (u32)table;
                    read = offset+1;
                    do {
                        read++;
                        count++;
                    } while (count <= steps && column+count <= 7 && *(const s8 *)read == kind);
                }
            }
        }
        {
            s32 next = before;
            column = next+count;
        }
    }
    {
        s32 width = SceneGrid_Width();
        s32 contribution = rowPlus;
        contribution *= width;
        {
            s32 offset = contribution;
            offset += 5;
            cell = column+offset;
        }
    }
    if (cell == blocked) {
        if (steps < 0) {
            s32 next = column-2;
            if (next < 0) goto noPosition;
            {
                const s8 *table = gUnk_08605BC8;
                register u32 offset asm("r1");
                offset = row*8;
                offset -= 2;
                offset = column+offset;
                offset += (u32)table;
                if (*(const s8 *)offset != kind) goto noPosition;
            }
            return 2-cell;
        } else {
            s32 next = column+2;
            if (next > 7) goto noPosition;
            {
                const s8 *table = gUnk_08605BC8;
                register u32 offset asm("r1");
                offset = row*8;
                offset += 2;
                offset = column+offset;
                offset += (u32)table;
                if (*(const s8 *)offset != kind) goto noPosition;
            }
            return -(cell+2);
        }
    }
    return cell;
}
