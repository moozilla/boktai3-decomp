#include "global.h"

struct S { u8 f00[0x424]; u16 h24; u16 h26; };
struct P { u8 f00[0xA40]; struct S *s; };

void sub_081642CC(struct P *, u32, u32, u32, u32, u32, u32);

void sub_08164438(struct P *p, u32 a, u32 b, u32 c, u32 d)
{
    sub_081642CC(p, a, b, c, d, p->s->h24, p->s->h26);
}
