#include "global.h"
struct S {
    u8 f[0x14];
    u8 c;
};
void PaletteFlash_SelectPalette(struct S *, u32);
void Character_UpdateTimedEffect(struct S *s)
{
    if (s->c != 0) {
        s->c--;
        if (s->c == 0)
            PaletteFlash_SelectPalette(s, 0);
    }
}
