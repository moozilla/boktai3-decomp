#include "global.h"
// Family and descriptive names are tentative interpretations of static code.
#define PaletteTransition_BlendSelectedBanks sub_081D427C
struct PaletteTransitionBanks {
    u16 source[16][16];
    u16 destination[16][16];
    u16 target[16][16];
    u8 pad600[2];
    u8 firstBank;
    u8 extraBank;
};
struct PaletteTransition {
    u8 pad0[0x3a0];
    struct PaletteTransitionBanks banks;
};
void sub_08221110(void *, void *, void *, int, int);
void sub_081D427C(struct PaletteTransition *p, int amount, int mode)
{
    struct PaletteTransitionBanks *b = &p->banks;
    s32 i;
    for (i = 0; i <= 2; i++) {
        int bank = b->firstBank + i;
        sub_08221110(b->destination[bank], b->source[bank], b->target, amount, mode);
    }
    sub_08221110(b->destination[b->extraBank], b->source[b->extraBank], b->target, amount, mode);
}
