#include "scene_grid.h"
/* The 10 x 8 board, placed at grid column 5, row 8. */
extern const s8 gUnk_08605BC8[][8];

/* Negative results encode a cell two steps beyond a blocked candidate. */
s32 SceneGrid_ScanRows(s32 column, s32 row, s32 blocked, s32 kind, s32 steps)
{
    s32 count;
    s32 cell;
    s32 width;
    s32 gridRow, rowBase;
    if (steps == 0) {
    noCell:
        return -255;
    }
    if (steps < 0) {
        for (count = 1;
             count <= -steps && row - count >= 0 && gUnk_08605BC8[row - count][column] == kind;
             count++)
            ;
        row++;
        row -= count;
    } else {
        for (count = 1;
             count <= steps && row + count <= 9 && gUnk_08605BC8[row + count][column] == kind;
             count++)
            ;
        row--;
        row += count;
    }
    gridRow = row + 8;
    width = SceneGrid_Width();
    rowBase = gridRow * width + 5;
    cell = column + rowBase;
    if (cell == blocked) {
        if (steps < 0) {
            if (row - 2 < 0 || gUnk_08605BC8[row - 2][column] != kind)
                goto noCell;
            return width * 2 - cell;
        } else {
            if (row + 2 > 9 || gUnk_08605BC8[row + 2][column] != kind)
                goto noCell;
            return -(cell + width * 2);
        }
    }
    return cell;
}
