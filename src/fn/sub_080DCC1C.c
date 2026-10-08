#include "global.h"

extern const u8 gUnk_086061DC[];

struct S { u8 filler[652]; const u8 *p; };

void sub_080DCC1C(struct S *s)
{
    s->p = gUnk_086061DC;
}
