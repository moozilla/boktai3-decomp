#include "global.h"

struct Q { u8 f[0x87c]; u16 v[6]; };
struct P { u8 f[0x3d0]; struct Q *q; };

s32 sub_080AB51C(struct P *p, u32 x)
{
    struct Q *q = p->q;
    s32 i = 0;
    u16 *v = q->v;
    s32 r;
    do {
        if (*v == x) {
            r = 1;
            goto done;
        }
        v++;
        i++;
    } while (i <= 5);
    r = 0;
done:
    return r;
}
