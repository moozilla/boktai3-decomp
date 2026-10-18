#include "global.h"

struct A {
    u8 pad[0x574];
    u32 counter;
};

extern u32 gUnk_0300523C;
void sub_081AD3E8(struct A *p);
void sub_081A8C9C(struct A *p);
void sub_081AD0D0(struct A *p);
void sub_081AD3C0(struct A *p);
void sub_081AD564(struct A *p);

void sub_081ADD0C(struct A *p)
{
    if ((gUnk_0300523C & 7) == 0) {
        sub_081AD3E8(p);
        sub_081A8C9C(p);
        sub_081AD0D0(p);
        sub_081AD3C0(p);
        sub_081AD564(p);
        p->counter = ((p->counter + 1) & 0xFFFF) + 1;
    }
}
