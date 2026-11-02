#include "armor_palette.h"
void Palette_CopyBank(u16 *, u16 *);
void ArmorPalette_CopyKindBanks(struct ArmorPaletteController *p, void *data, u32 first, u32 extra)
{
    u16 *from;
    u16 *to;
    u8 *index;
    from = (u16 *)((u8 *)data + (first << 5));
    index = &p->bank;
    to = p->palettes[*index];
    Palette_CopyBank(to, from);
    from = (u16 *)((u8 *)data + ((first + 1) << 5));
    to = p->palettes[*index + 1];
    Palette_CopyBank(to, from);
    from = (u16 *)((u8 *)data + ((first + 2) << 5));
    to = p->palettes[*index + 2];
    Palette_CopyBank(to, from);
    from = (u16 *)((u8 *)data + (extra << 5));
    to = p->palettes[p->otherBank];
    Palette_CopyBank(to, from);
}
