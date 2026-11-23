#include "global.h"
struct S {
    u8 f[0x724];
    u16 a;
    u16 b[18];
};
void LinkScorePanel_InitGlyphFrames(struct S *s)
{
    s32 i;
    s->a = 0x3d;
    for (i = 0; i < 18; i++)
        s->b[i] = 0x3e;
}
