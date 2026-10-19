#include "global.h"
// Family and descriptive names are tentative interpretations of static code.
#define PaletteTransition_BlendAccentColors sub_081D42E4
#define PaletteTransition_BlendColorRange sub_081D41F4
struct PaletteTransitionBanks {
    u16 colors[768];
    u8 pad600[2];
    u8 firstBank;
    u8 extraBank;
};
struct PaletteTransition {
    u8 pad0[0x3a0];
    struct PaletteTransitionBanks banks;
};
void sub_081D41F4(void *, void *, void *, int, int);
void sub_081D42E4(struct PaletteTransition *p, int amount)
{
    u16 *b = p->banks.colors;
    u8 *bank = &p->banks.firstBank;
    sub_081D41F4((b + *bank * 16) + 0x111, (b + *bank * 16) + 17, p->banks.colors + 0x211, amount, 6);
    sub_081D41F4((b + *bank * 16) + 0x121, (b + *bank * 16) + 33, p->banks.colors + 0x211, amount, 6);
}
