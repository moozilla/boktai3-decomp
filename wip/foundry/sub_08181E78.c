#include "global.h"

#define ArmorPalette_StepFlashCycle sub_08181E78

struct ArmorPaletteController {
    void (*update)(void *);
    void (*finish)(void *);
    u16 source[16][16]; /* 0x008 */
    u16 banks[16][16];  /* 0x208 */
    u16 saved[4][16];   /* 0x408 */
    s16 timer;          /* 0x488 */
    u8 bank;            /* 0x48a */
    u8 otherBank;       /* 0x48b */
};

void Palette_BlendBank(void *dest, void *source, int count, int step, int steps);
void Palette_CopyBank(void *dest, u32 source);
void Palette_InterpolateBanks(void *dest, void *from, void *to, int step, int steps);

void sub_08181E78(struct ArmorPaletteController *c)
{
    u8 *dest;
    u8 *source;
    u16 *saved;
    u32 off;
    u32 savedOff;
    s32 step;

    if (c->timer <= 8) {
        off = c->bank * 32;
        source = (u8 *)c + (off + 8);
        dest = (u8 *)c + (off + 0x208);
        Palette_BlendBank(dest, source, 0x10, c->timer, 3);
        off = c->bank * 32;
        source = (u8 *)c + (off + 0x28);
        dest = (u8 *)c + (off + 0x228);
        Palette_BlendBank(dest, source, 0x10, c->timer, 3);
        off = c->bank * 32;
        source = (u8 *)c + (off + 0x48);
        dest = (u8 *)c + (off + 0x248);
        Palette_BlendBank(dest, source, 0x10, c->timer, 3);
        off = c->otherBank * 32;
        source = (u8 *)c + (off + 8);
        dest = (u8 *)c + (off + 0x208);
        Palette_BlendBank(dest, source, 0x10, c->timer, 3);
        if (c->timer == 8) {
            off = c->bank * 32;
            savedOff = 0x408;
            Palette_CopyBank((u8 *)c + savedOff, (u32)((u8 *)c + (off + 0x208)));
            off = c->bank * 32;
            savedOff += 0x20;
            Palette_CopyBank((u8 *)c + savedOff, (u32)((u8 *)c + (off + 0x228)));
            off = c->bank * 32;
            savedOff += 0x20;
            Palette_CopyBank((u8 *)c + savedOff, (u32)((u8 *)c + (off + 0x248)));
            off = c->otherBank * 32;
            savedOff += 0x20;
            Palette_CopyBank((u8 *)c + savedOff, (u32)((u8 *)c + (off + 0x208)));
        }
    } else {
        step = 0x10 - c->timer;
        source = (u8 *)c + (c->bank * 32 + 8);
        saved = c->saved[0];
        dest = (u8 *)c + (c->bank * 32 + 0x208);
        Palette_InterpolateBanks(dest, saved, source, step, 3);
        source = (u8 *)c + (c->bank * 32 + 0x28);
        saved = c->saved[1];
        dest = (u8 *)c + (c->bank * 32 + 0x228);
        Palette_InterpolateBanks(dest, saved, source, step, 3);
        source = (u8 *)c + (c->bank * 32 + 0x48);
        saved = c->saved[2];
        dest = (u8 *)c + (c->bank * 32 + 0x248);
        Palette_InterpolateBanks(dest, saved, source, step, 3);
        source = (u8 *)c + (c->otherBank * 32 + 8);
        saved = c->saved[3];
        dest = (u8 *)c + (c->otherBank * 32 + 0x208);
        Palette_InterpolateBanks(dest, saved, source, step, 3);
    }
    c->timer++;
    if (c->timer > 0xf)
        c->timer = 0;
}
