#include "global.h"

struct S { u8 pad[0x68]; s32 f68; };
void sub_081C439C(struct S *);
void sub_081C4524(struct S *);

void sub_081C4558(struct S *p)
{
    sub_081C439C(p);
    if (p->f68 > 0xb3) {
        sub_081C4524(p);
    }
    p->f68++;
}
