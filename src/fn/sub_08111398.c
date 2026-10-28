#include "global.h"
#include "render.h"
struct S {
    u8 f[0x1844];
    u16 a;
    u16 pad;
    u16 b;
    u16 pad2;
    u8 c;
};
void BackgroundScroll_UpdateHorizontal(struct S *s)
{
    s->b += s->a;
    gBgRecords[s->c].x = s->b >> 7;
}
