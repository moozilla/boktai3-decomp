#include "global.h"

struct A { u8 f[0x5c7]; u8 m; };
void sub_081A7A0C(void *);
void sub_081A7AD0(void *);

void sub_081A7BE0(struct A *p)
{
    switch (p->m) {
    case 1:
        sub_081A7A0C(p);
        break;
    case 2:
    case 3:
        sub_081A7AD0(p);
        break;
    }
}
