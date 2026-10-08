#include "global.h"

extern const u8 gUnk_086063E4[];
extern const u8 gUnk_086063D4[];
extern const u8 gUnk_086063F4[];

struct P { const u8 *a; u32 pad; };
struct S { u8 filler[0x284]; struct P p[3]; };

void sub_080F02F4(struct S *s)
{
    s->p[0].a = gUnk_086063E4;
    s->p[1].a = gUnk_086063D4;
    s->p[2].a = gUnk_086063F4;
}
