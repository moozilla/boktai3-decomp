#include "global.h"

struct S { u8 pad[0x68]; s32 f68; };
void sub_081C439C(struct S *);
void sub_081C44C0(struct S *);

void sub_081C46A0(struct S *p)
{
    sub_081C439C(p);
    p->f68++;
    if (p->f68 > 0x3f) {
        sub_081C44C0(p);
    }
}
