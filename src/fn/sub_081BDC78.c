#include "global.h"
#include "scene_input_grid.h"

/* Tentative storage navigation: two 4x4 groups and four category cells. */
void Storage_RebuildGrid(struct SceneInputGrid *p)
{
    s32 i, j;
    {
        for (i = 0; i <= 3; i++)
            for (j = 0; j <= 3; j++)
                p->cells[j + 4][i] |= 0xFFu;
    }
    if (!SceneInput_TestGridFlags(p, 1)) {
        for (i = 0; i <= 3; i++) {
            u32 offset = i * 4;
            s32 value;
            j = 3;
            value = offset + 3;
            for (; j >= 0; j--)
                p->cells[j + 4][i] = value--;
        }
    }
    {
        for (i = 0; i <= 3; i++)
            for (j = 0; j <= 3; j++)
                p->cells[j][i] |= 0xFFu;
    }
    if (!SceneInput_TestGridFlags(p, 2)) {
        for (i = 0; i <= 3;) {
            s32 next = i + 1;
            u32 offset = i * 4;
            s32 value;
            j = 3;
            value = offset + 0x13;
            for (; j >= 0; j--)
                p->cells[j][i] = value--;
            i = next;
        }
    }
    if (SceneInput_TestGridFlags(p, 0x10)) {
        for (i = 0; i <= 3; i++)
            p->cells[8][i] |= 0xFFu;
    } else {
        p->cells[8][0] = -1;
        p->cells[8][1] = 0x22;
        p->cells[8][2] = 0x21;
        p->cells[8][3] = 0x20;
    }
}
