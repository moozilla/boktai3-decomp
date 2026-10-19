/* Provisional names and object interpretation; offsets follow static accesses. */
#define MenuNavigation_InitializeGrid sub_0816CDB8
#include "global.h"
/* Near match: population-loop scheduling and optional-store branch layout differ. */
struct MenuNavigationCell {
    s8 value : 8;
} __attribute__((packed));
struct MenuNavigation {
    u8 flags;
    u8 pad1[11];
    union {
        struct MenuNavigationCell entries[36];
        struct {
            u8 grid[32];
            s8 special[4];
        } named;
    } cells;
};
void sub_0816CDB8(struct MenuNavigation *s, s32 populate, s32 flags)
{
    s32 column, row;
    struct MenuNavigationCell *base;
    s->flags = flags;
    column = 0;
    base = s->cells.entries;
    for (; column <= 3;) {
        s32 next = column + 1;
        for (row = 0; row <= 8; row++)
            base[row * 4 + column].value = -1;
        column = next;
    }
    if (populate) {
        for (column = 0; column <= 3;) {
            s32 next = column + 1;
            s32 value;
            row = 3;
            value = column * 4 + row;
            for (; row >= 0; row--) {
                base[row * 4 + column].value = value;
                value--;
            }
            column = next;
        }
    }
    if (flags & 1)
        s->cells.named.special[0] = 0x20;
    else
        *(u8 *)&s->cells.named.special[0] = 255;
    if (flags & 0x20)
        s->cells.named.special[1] = 0x26;
    else
        *(u8 *)&s->cells.named.special[1] = 255;
    if (flags & 0x40)
        s->cells.named.special[2] = 0x27;
    else
        *(u8 *)&s->cells.named.special[2] = 255;
    if (flags & 0x80)
        s->cells.named.special[3] = 0x28;
    else
        *(u8 *)&s->cells.named.special[3] = 255;
}
