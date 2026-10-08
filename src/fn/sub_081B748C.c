#include "global.h"

struct E { u8 f[0x4c]; };
struct A { u8 f[0x20]; struct E e[8]; };
void sub_08214514(void *);

void sub_081B748C(struct A *p)
{
    s32 i;
    struct E *e = p->e;
    for (i = 7; i >= 0; i--) {
        sub_08214514(e);
        e++;
    }
}
