#include "global.h"

struct S { u8 pad[0x1c]; s16 x; s16 y; };
void sub_08216520(u32, s32, s32);

void sub_081C439C(struct S *p)
{
    sub_08216520(3, p->x, p->y);
}
