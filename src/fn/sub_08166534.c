#include "global.h"
#ifdef NONMATCHING
// twin.py transplant from a matched near-clone
struct HudPos {
    s16 x, y, z, pad;
};
struct HudSprite {
    u8 pad00[8];
    u32 flags;
    u8 pad0C[0x14];
    struct HudPos pos;
    u8 pad28[0x38];
};
struct HudStats {
    u8 pad[0x332];
    u16 bonus[3];
    u8 pad338[0x41C - 0x338];
    u16 values[3];
};
struct HudPanel {
    u8 pad0000[0x64];
    u8 digits[0xA40 - 0x64];
    struct HudStats *stats;
    u8 padA44[0x2B60 - 0xA44];
    struct HudSprite numbers[9];
    u8 pad2EC0[0x48AC - 0x2EC0];
    u8 shown[4];
    s16 base[3];
    s16 delta[3];
};
u16 *Hud_GetTilemapCell(s32, s32, s32);
void MenuDisplay_LoadAdjustedDigits(s32, s32, void *, void *, void *, void *);
static inline void SetPos(struct HudPos *pos, s32 x, s32 y)
{
    pos->x = x;
    pos->y = y;
    pos->z = 0;
}
static inline void Show(struct HudSprite *s)
{
    s->flags &= ~1;
}
void sub_08166534(struct HudPanel *p, s32 col, s32 row)
{
    s32 i;
    s32 j;
    u16 *cell;
    for (i = 0; i <= 2; i++) {
        p->shown[i] = p->stats->values[i];
        if (p->shown[i] > 100)
            p->shown[i] = 100;
        cell = Hud_GetTilemapCell(0, col, row + i);
        for (j = 0; j < 3; j++)
            *(cell++) = 0xF001;

        MenuDisplay_LoadAdjustedDigits(p->shown[i] + p->base[i], p->delta[i], p->digits,
                                       &p->numbers[(i * 3) + 0], &p->numbers[(i * 3) + 1],
                                       &p->numbers[(i * 3) + 2]);
        SetPos(&p->numbers[(i * 3) + 0].pos, col * 8, (row + i) * 8);
        SetPos(&p->numbers[(i * 3) + 1].pos, (col * 8) + 8, (row + i) * 8);
        SetPos(&p->numbers[(i * 3) + 2].pos, (col * 8) + 16, (row + i) * 8);
        {
            struct HudPanel *q = (struct HudPanel *)((i * 0x120) + ((u32)p));
            Show(&q->numbers[0]);
            Show(&q->numbers[1]);
            Show(&q->numbers[2]);
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08166534);
#endif
