#include "scene_grid.h"
/* The 10 x 8 board, placed at grid column 5, row 8. */
extern const s8 gUnk_08605BC8[][8];

/* Negative results encode a cell two steps beyond a blocked candidate. */
s32 SceneGrid_ScanColumns(s32 column, s32 row, s32 blocked, s32 kind, s32 steps)
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
        for (count = 1; count <= -steps && column - count >= 0 && gUnk_08605BC8[row][column - count] == kind; count++)
            ;
        column++;
        column -= count;
    } else {
        for (count = 1; count <= steps && column + count <= 7 && gUnk_08605BC8[row][column + count] == kind; count++)
            ;
        column--;
        column += count;
    }
    gridRow = row + 8;
    width = SceneGrid_Width();
    rowBase = gridRow * width + 5;
    cell = column + rowBase;
    if (cell == blocked) {
        if (steps < 0) {
            if (column - 2 < 0 || gUnk_08605BC8[row][column - 2] != kind)
                goto noCell;
            return 2 - cell;
        } else {
            if (column + 2 > 7 || gUnk_08605BC8[row][column + 2] != kind)
                goto noCell;
            return -(cell + 2);
        }
    }
    return cell;
}
