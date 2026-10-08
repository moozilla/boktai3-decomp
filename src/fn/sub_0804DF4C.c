#include "global.h"
struct E { u8 f[0xc6]; u8 on; u8 a[5]; u16 x0; u16 y0; u16 x1; u16 y1; };
struct O { u8 f[0x26]; u8 n; };
extern struct O *gUnk_0200010C;
struct P { s16 x; s16 pad; s16 y; };
s32 sub_0804DF4C(struct P *p)
{
    struct O *o = gUnk_0200010C;
    s32 i;
    s32 n;
    if (o != 0) goto go;
    goto no;
yes:
    return 1;
go:
    i = 0;
    if (i >= o->n) goto no;
    n = o->n;
    do {
        struct E *e = (struct E *)((u8 *)o + i * 0xa8);
        if (e->on != 0) {
            s32 x = p->x;
            if (x >= e->x0) {
                s32 y = p->y;
                if (y >= e->y0) {
                    if (x <= e->x1) {
                        if (y <= e->y1) goto yes;
                    }
                }
            }
        }
        i++;
    } while (i < n);
no:
    return 0;
}
