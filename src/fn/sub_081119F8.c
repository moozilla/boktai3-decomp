#include "global.h"
#include "ram.h"
struct S {
    u8 f[0x1844];
    u16 a;
    u16 b;
};
void PaletteFade_SetScrollPosition(s32 a, s32 b)
{
    struct S *s = gPaletteFade;
    if (s != 0) {
        s->a = a;
        s->b = b;
    }
}
