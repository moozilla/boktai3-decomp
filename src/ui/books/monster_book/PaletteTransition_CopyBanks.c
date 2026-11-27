#include "global.h"
struct PaletteTransitionBanks {
    u16 source[16][16];
    u16 destination[16][16];
    u16 target[16][16];
    u8 pad600[2];
    u8 firstBank;
    u8 extraBank;
};
void Palette_CopyBank(u16 *, u16 *);
void PaletteTransition_CopyBanks(struct PaletteTransitionBanks *p, u8 *data, u32 first, u32 extra)
{
    u16 *from;
    u16 *to;
    u8 *index;
    from = (u16 *)((u8 *)data + (first << 5));
    index = &p->firstBank;
    to = p->source[*index];
    Palette_CopyBank(to, from);
    from = (u16 *)((u8 *)data + ((first + 1) << 5));
    to = p->source[*index + 1];
    Palette_CopyBank(to, from);
    from = (u16 *)((u8 *)data + ((first + 2) << 5));
    to = p->source[*index + 2];
    Palette_CopyBank(to, from);
    from = (u16 *)((u8 *)data + (extra << 5));
    to = p->source[p->extraBank];
    Palette_CopyBank(to, from);
}
