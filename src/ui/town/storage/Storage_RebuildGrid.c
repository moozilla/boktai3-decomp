#include "global.h"
#include "ui/widgets/scene_input_grid.h"

/* Tentative storage navigation: two 4x4 groups and four category cells. */
void Storage_RebuildGrid(struct SceneInputGrid *p)
{
    s32 i, j;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++)
            p->cells[j + 4][i] = -1;
    }
    if (!SceneInput_TestGridFlags(p, 1)) {
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++)
                p->cells[j + 4][i] = i * 4 + j;
        }
    }
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++)
            p->cells[j][i] = -1;
    }
    if (!SceneInput_TestGridFlags(p, 2)) {
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++)
                p->cells[j][i] = i * 4 + j + 16;
        }
    }
    if (SceneInput_TestGridFlags(p, 0x10)) {
        for (i = 0; i < 4; i++)
            p->cells[8][i] = -1;
    } else {
        p->cells[8][0] = -1;
        p->cells[8][1] = 0x22;
        p->cells[8][2] = 0x21;
        p->cells[8][3] = 0x20;
    }
}
