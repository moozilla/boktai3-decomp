#include "global.h"

struct P { u8 f00[0x3A7]; u8 c; u8 f3a8[0x3A8 - 0x3A8]; };

void sub_0821FE40(void *);

void sub_0815DEC4(struct P *p)
{
    u8 *q = &p->c;
    if (*q != 0) {
        sub_0821FE40((u8 *)p + 0x3A8);
        *q = *q - 1;
    }
}
