#include "scene_input_grid.h"
#ifdef NONMATCHING
// fuzzy: 97.9%
// Five FF constant birth orders differ (480/480 bytes).
// MATCHING_CONCESSION: register-pinning
#define SceneInput_RebuildGrid sub_0805E5E4
void SceneInput_RebuildGrid(struct SceneInputGrid *grid)
{
    s32 column, row;
    {
        register u32 fill asm("r4");
        for (column = 0, fill = 0xFF; column <= 3; column++)
            for (row = 0; row <= 3; row++)
                grid->cells[(row) + 4][column] |= fill;
    }
    if (!SceneInput_TestGridFlags(grid, 1)) {
        for (column = 0; column <= 3; column++) {
            u32 offset = column * 4;
            s32 value;
            asm("" : "+r"(offset) : : "r2");
            row = 3;
            value = offset + 3;
            for (; row >= 0; row--)
                grid->cells[(row) + 4][column] = value--;
        }
    }
    {
        register u32 fill asm("r6");
        for (column = 0, fill = 0xFF; column <= 3; column++)
            for (row = 0; row <= 3; row++)
                grid->cells[row][column] |= fill;
    }
    if (!SceneInput_TestGridFlags(grid, 2)) {
        if (grid->kind & 1) {
            if (SceneInput_TestGridFlags(grid, 0x20)) {
                for (column = 0; column <= 3; column++)
                    for (row = 0; row <= 3; row++)
                        grid->cells[row][column] = grid->fallback;
            } else {
                for (column = 0; column <= 1;) {
                    s32 next = column + 1;
                    u32 offset = column * 4;
                    s32 value;
                    asm("" : "+r"(offset) : "r"(next) : "r2");
                    row = 3;
                    value = offset + 19;
                    for (; row >= 0; row--)
                        grid->cells[row][column] = value--;
                    column = next;
                }
            }
        } else {
            for (column = 0; column <= 2;) {
                s32 next = column + 1;
                u32 offset = column * 4;
                s32 value;
                asm("" : "+r"(offset) : "r"(next) : "r2");
                row = 3;
                value = offset + 19;
                for (; row >= 0; row--)
                    grid->cells[row + 4][column] = value--;
                column = next;
            }
            {
                s32 value;
                row = 3;
                value = 31;
                for (; row >= 0; row--)
                    grid->cells[row + 4][3] = value--;
            }
        }
    }
    if (SceneInput_TestGridFlags(grid, 0x10)) {
        if (SceneInput_TestGridFlags(grid, 8)) {
            {
                register u32 fill asm("r4");
                for (column = 0, fill = 0xFF; column <= 3; column++)
                    grid->cells[8][column] |= fill;
            }
        } else {
            {
                register u32 fill asm("r4");
                for (column = 0, fill = 0xFF; column <= 3; column++)
                    grid->cells[8][column] |= fill;
            }
            grid->cells[8][2] = 25;
        }
    } else if (grid->kind == 1) {
        {
            register u32 fill asm("r4");
            for (column = 0, fill = 0xFF; column <= 3; column++)
                grid->cells[8][column] |= fill;
        }
        if (!SceneInput_TestGridFlags(grid, 8))
            grid->cells[8][2] = 25;
    } else {
        grid->cells[8][0] = 26;
        grid->cells[8][1] = 0xFF;
        grid->cells[8][2] = -1;
        if (SceneInput_TestGridFlags(grid, 8))
            grid->cells[8][3] |= -1;
        else
            grid->cells[8][3] = 25;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0805E5E4);
#endif
