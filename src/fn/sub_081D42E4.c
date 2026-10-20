#include "global.h"
#define PaletteTransition_BlendBankColors sub_081D42E4
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
void sub_081D41F4(u16 *, u16 *, u16 *, int, int);
void PaletteTransition_BlendBankColors(struct PaletteTransition *p, int weight)
{
    struct PaletteTransitionBanks *b = &p->banks;
    sub_081D41F4(&b->destination[b->firstBank + 1][1], &b->source[b->firstBank + 1][1],
                 &b->target[1][1], weight, 6);
    sub_081D41F4(&b->destination[b->firstBank + 2][1], &b->source[b->firstBank + 2][1],
                 &b->target[1][1], weight, 6);
}
