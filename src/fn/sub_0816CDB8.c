#include "global.h"
#include "scene_input_grid.h"

void sub_0816CDB8(struct SceneInputGrid *grid, s32 identity, u32 kind)
{
    s32 i, j;
    grid->kind = kind;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 9; j++)
            grid->cells[j][i] |= 0xFFu;
    if (identity) {
        for (i = 0; i < 4; i++)
            for (j = 0; j < 4; j++)
                grid->cells[j][i] = i * 4 + j;
    }
    if (kind & 1)
        grid->cells[8][0] = 0x20;
    else
        grid->cells[8][0] = 0xFF;
    if (kind & 0x20)
        grid->cells[8][1] = 0x26;
    else
        grid->cells[8][1] = 0xFF;
    if (kind & 0x40)
        grid->cells[8][2] = 0x27;
    else
        grid->cells[8][2] = 0xFF;
    if (kind & 0x80)
        grid->cells[8][3] = 0x28;
    else
        grid->cells[8][3] = 0xFF;
}
