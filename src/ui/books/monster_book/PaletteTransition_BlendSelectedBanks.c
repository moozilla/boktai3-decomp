#include "global.h"
#include "ui/books/palette_transition_banks.h"
// Family and descriptive names are tentative interpretations of static code.
struct PaletteTransition {
    u8 pad0[0x3a0];
    struct PaletteTransitionBanks banks;
};
void Palette_InterpolateBanks(void *, void *, void *, int, int);
void PaletteTransition_BlendSelectedBanks(struct PaletteTransition *p, int amount, int mode)
{
    struct PaletteTransitionBanks *b = &p->banks;
    s32 i;
    for (i = 0; i <= 2; i++) {
        int bank = b->firstBank + i;
        Palette_InterpolateBanks(b->destination[bank], b->source[bank], b->target, amount, mode);
    }
    Palette_InterpolateBanks(b->destination[b->extraBank], b->source[b->extraBank], b->target,
                             amount, mode);
}
