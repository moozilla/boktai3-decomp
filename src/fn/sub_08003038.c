#include "palette_controller.h"

void sub_08002C38(struct PaletteController *, struct PaletteTransition *);

void sub_08003038(struct PaletteController *p)
{
    s32 i = 3;
    u16 *v = p->palettes[3];
    u16 **t = &p->colorSets[PALETTE_SET_VARIANT][3];
    do {
        *t = v;
        v -= 256;
        t--;
        i--;
    } while (i >= 0);
    sub_08002C38(p, &p->transition);
}
