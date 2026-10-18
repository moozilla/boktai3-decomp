#include "global.h"
void Palette_InterpolateBanks(void *, void *, void *, int, int);
void Vanargand_BlendPaletteBanks(u8 *a, u8 *b, u8 *c, int d, int e, int n)
{
    if (n > 0) {
        u8 *z = c;
        u8 *y = b;
        u8 *x = a;
        int count = n;
        /* do-while(0) only keeps agbcc's block layout */
        do {
            Palette_InterpolateBanks(x, y, z, d, e);
            z += 0x20;
            y += 0x20;
            /* do-while(0) only keeps agbcc's block layout */
            do {
                x += 0x20;
            } while (0);
        } while (--count);
    }
}
