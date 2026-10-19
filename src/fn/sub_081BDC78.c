
#ifdef NONMATCHING
// fuzzy: 97.6%
// foundry draft (worker best 97.6%); what differs is mostly register allocation and instruction order
#define Storage_RebuildGrid sub_081BDC78
#include "global.h"
#include "scene_input_grid.h"

/* Tentative storage navigation: two 4x4 groups and four category cells. */
void sub_081BDC78(struct SceneInputGrid *p)
{
    s32 i, j;
    {
        s32 fill;
        for (i = 0; i <= 3; i++)
            for (j = 0, fill = 0xFF; j <= 3; j++) {
                u32 value = (u8)p->cells[j + 4][i];
                value |= fill;
                p->cells[j + 4][i] = value;
            }
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
        s32 fill;
        for (i = 0; i <= 3; i++)
            for (j = 0, fill = 0xFF; j <= 3; j++) {
                u32 value = (u8)p->cells[j][i];
                value |= fill;
                p->cells[j][i] = value;
            }
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
        s32 fill;
        for (i = 0; i <= 3; i++) {
            u32 value;
            fill = 0xFF;
            value = (u8)p->cells[8][i];
            value |= fill;
            p->cells[8][i] = value;
        }
    } else {
        p->cells[8][0] = -1;
        p->cells[8][1] = 0x22;
        p->cells[8][2] = 0x21;
        p->cells[8][3] = 0x20;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081BDC78);
#endif
