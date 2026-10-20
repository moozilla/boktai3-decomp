#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct PaletteTransitionBanks {
    u16 source[16][16];
    u16 destination[16][16];
    u16 target[16][16];
    u8 pad600[2];
    u8 firstBank;
    u8 extraBank;
};
void Palette_CopyBank(void *, void *);
void PaletteTransition_CopyBanks(struct PaletteTransitionBanks *c, u8 *src, u32 bank, u32 extra)
{
    register u8 *first asm("r5");
    u8 *s;
    s = src + (bank << 5);
    first = &c->firstBank;
    Palette_CopyBank((u8 *)c + (*first << 5), s);
    s = src + ((bank + 1) << 5);
    Palette_CopyBank((u8 *)c + ((*first << 5) + 0x20), s);
    s = src + ((bank + 2) << 5);
    Palette_CopyBank((u8 *)c + ((*first << 5) + 0x40), s);
    extra <<= 5;
    s = src + extra;
    {
        u8 *d = (u8 *)c + (c->extraBank << 5);
        Palette_CopyBank(d, s);
    }
    asm("" : : "r"(c));
}
