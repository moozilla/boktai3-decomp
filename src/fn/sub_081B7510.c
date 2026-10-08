#include "global.h"

struct A { u8 f[0x18]; s32 n; };
void *sub_081B7340(void *);
void sub_081B7378(void *, void *);

s32 sub_081B7510(struct A *p, void *q)
{
    void *r = sub_081B7340(p);
    if (r != 0) {
        sub_081B7378(r, q);
        p->n++;
        return 1;
    }
    return 0;
}
