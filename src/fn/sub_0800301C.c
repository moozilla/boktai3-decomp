#include "palette_controller.h"

void sub_0800301C(struct PaletteController *p)
{
    u16 **q = p->colorSets[PALETTE_SET_SOURCE];
    s32 i = 3;
    do {
        q[4 * PALETTE_SET_VARIANT] = *q; // copy source slot to the matching variant slot
        q++;
        i--;
    } while (i >= 0);
}
