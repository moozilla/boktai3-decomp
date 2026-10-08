#include "global.h"

extern const u8 gUnk_08606134[];

struct S { u8 filler[0x288]; const u8 *p; };

void sub_080CC820(struct S *s)
{
    s->p = gUnk_08606134;
}
