#include "global.h"

struct P { s16 x; s16 pad; s16 y; };
struct A { u8 f[0x20]; s16 x; s16 pad; s16 y; };
struct P *sub_08049B6C(void);
s32 sub_082215E4(s32, s32);
void sub_0804A0CC(s32, s32);

void sub_081A4B80(struct A *p)
{
    struct P *q = sub_08049B6C();
    s32 dx = q->x - p->x;
    s32 dy = q->y - p->y;
    if (dx * dx + dy * dy <= 0xFFFF)
        sub_0804A0CC(0x80, sub_082215E4(dx, dy));
}
