#include "global.h"
#include "ui/books/palette_transition_banks.h"
void Palette_CopyBank(u16 *, u16 *);
void PaletteTransition_CopyBanks(struct PaletteTransitionBanks *p, u16 *data, u32 first, u32 extra)
{
    u16 *from;
    u16 *to;
    from = data + (first << 4);
    to = p->source[p->firstBank];
    Palette_CopyBank(to, from);
    from = data + ((first + 1) << 4);
    to = p->source[p->firstBank + 1];
    Palette_CopyBank(to, from);
    from = data + ((first + 2) << 4);
    to = p->source[p->firstBank + 2];
    Palette_CopyBank(to, from);
    from = data + (extra << 4);
    to = p->source[p->extraBank];
    Palette_CopyBank(to, from);
}
