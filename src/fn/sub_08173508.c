#include "global.h"
/* SceneInput_BuildActionGrid_08173508: SceneInput_BuildActionGrid with other constants (ported by relatives.py, fixed by hand; provisional name) */
#include "scene_input_navigation.h"
void SceneInput_BuildActionGrid_08173508(struct SceneInputGridView *grid, u32 flags)
{
    s32 row, col;
    u8 *cells;
    u16 mask;
    u8 *slot0, *slot1, *slot2, *slot3;
    grid->kind = flags;
    row = 0;
    cells = (u8 *)grid->cells;
    slot0 = cells + 32;
    slot1 = cells + 33;
    slot2 = cells + 34;
    slot3 = cells + 35;
    mask = 0xff;
    for (; row < 9; row++) {
        u8 *p = (u8 *)(row + (u32)cells);
        for (col = 3; col >= 0; col--) {
            u32 value = *p;
            *p = value | mask;
            p += 4;
        }
    }
    row = 0;
    {
        u8 *entries = cells;
        for (; row < 4;) {
            s32 next = row + 1;
            u32 base = row * 4;
            u32 value;
            u8 *p;
            col = 3;
            value = base + col;
            p = (u8 *)(row + (u32)entries) + 12;
            for (; col >= 0; col--) {
                *p = value--;
                p -= 4;
            }
            row = next;
        }
    }
    *slot0 = flags & 1 ? 0x20 : 0xff;
    if (flags & 0x20)
        *slot1 = 0x26;
    else {
        u16 empty = 0xff;
        *slot1 = empty;
    }
    if (flags & 0x40)
        *slot2 = 0x27;
    else {
        u16 empty = 0xff;
        *slot2 = empty;
    }
    if (flags & 0x80)
        *slot3 = 0x28;
    else {
        u16 empty = 0xff;
        *slot3 = empty;
    }
}
