#include "global.h"
/* NameEntry_UpdateCursor: positions the keyboard cursor sprite for the selected key (80 character cells in two 5x8 blocks, then the page/case/back/end buttons) */
#define NameEntry_UpdateCursor sub_081DCFE4

struct NameEntryAnim {
    u8 pad0[8];
    u32 flags;
    u8 padC[0x20 - 0xC];
    s16 x;
    s16 y;
    u8 pad24[0x88 - 0x24];
};

struct NameEntry {
    u8 task[0x18];
    struct NameEntryAnim anims[6];
    u8 pad348[4];
    s32 cursor;
};

extern const u8 gUnk_0824FCBC[];
extern const u8 gUnk_0824FCC4[];
void memcpy(void *, const void *, u32);
s32 Mod(s32, s32);
s32 Div(s32, s32);

void NameEntry_UpdateCursor(struct NameEntry *p)
{
    u8 columns[8];
    u8 rows[8];
    s32 col, row;

    memcpy(columns, gUnk_0824FCBC, 8);
    memcpy(rows, gUnk_0824FCC4, 8);
    p->anims[1].flags &= ~1;
    p->anims[2].flags |= 1;
    p->anims[5].flags |= 1;
    if (p->cursor < 80) {
        col = Mod(p->cursor, 5);
        if (p->cursor < 40) {
            row = Div(p->cursor, 5);
            p->anims[1].x = columns[col];
            p->anims[1].y = rows[row];
        } else {
            row = Div(p->cursor - 40, 5);
            p->anims[1].x = columns[col] + 0x58;
            p->anims[1].y = rows[row];
        }
    } else {
        switch (p->cursor - 80) {
            case 0:
                p->anims[1].x = 0xB0;
                p->anims[1].y = 0;
                break;
            case 1:
                p->anims[1].x = 0xC0;
                p->anims[1].y = 0;
                break;
            case 2:
                p->anims[1].x = 0xB0;
                p->anims[1].y = 0x10;
                break;
            case 3:
                p->anims[1].x = 0xC0;
                p->anims[1].y = 0x10;
                break;
            case 4:
                p->anims[2].x = 0;
                p->anims[2].y = -16;
                p->anims[2].flags &= ~1;
                p->anims[1].flags |= 1;
                break;
            case 5:
                p->anims[2].x = 0;
                p->anims[2].y = 0;
                p->anims[1].flags |= 1;
                p->anims[2].flags &= ~1;
                break;
            case 6:
                p->anims[5].x = 0;
                p->anims[5].y = 0;
                p->anims[1].flags |= 1;
                p->anims[5].flags &= ~1;
                break;
        }
    }
}
