#include "global.h"
#include "ram.h"
struct S {
    u8 f[0x1848];
    u16 a;
    u8 p[6];
    s16 h;
    u16 h2;
    u32 w;
};
s32 Script_GetKeywordValue(s32, s32);
s32 Div(s32, s32);
void PaletteFade_StartScrollFromScript(void)
{
    struct S *g = gPaletteFade;
    if (g != 0) {
        struct S *s = g;
        s->h = Script_GetKeywordValue(0x66, 0);
        s->h2 = Script_GetKeywordValue(0x70, 0) << 7;
        if (s->h == 0)
            s->w = s->h2;
        else
            s->w = Div(s->h2 - s->a, s->h);
        *((u8 *)s + 0x184D) = 1;
    }
}
