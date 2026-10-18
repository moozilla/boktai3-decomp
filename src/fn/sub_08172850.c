#define SceneHud_ConfigureActionGrid sub_08172850
#include "global.h"
#include "scene_hud_display.h"
static inline u8 GridOption(u32 flags, u32 mask, u32 value)
{
    if (flags & mask)
        return value;
    return 0xFF;
}
void sub_08172850(struct SceneHudActionGrid *p, u32 flags, u32 mode)
{
    s32 column, row, next;
    p->flags = flags;
    for (column = 0; column <= 3; column++) {
        for (row = 0; row <= 8; row++)
            p->cells[row][column] |= 0xFF;
    }
    if (mode <= 1) {
        for (column = 0; column <= 3; column = next) {
            s32 value, doubled;
            next = column + 1;
            doubled = column * 2;
            row = 1;
            value = doubled + 1;
            for (; row >= 0; row--)
                p->cells[row][column] = value--;
        }
    }
    if (mode == 2 || mode == 0) {
        for (column = 0; column <= 3; column = next) {
            s32 value;
            row = 2;
            next = column + 1;
            value = column * 3 + 6;
            for (; row <= 4; row++)
                p->cells[row][column] = row + value;
        }
    }
    p->cells[8][0] = GridOption(flags, 1, 0x20);
    p->cells[8][1] = GridOption(flags, 0x20, 0x26);
    p->cells[8][2] = GridOption(flags, 0x40, 0x27);
    p->cells[8][3] = GridOption(flags, 0x80, 0x28);
}
