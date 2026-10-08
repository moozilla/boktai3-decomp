#include "global.h"
struct N { u8 f[0x18]; struct N *next; u8 g[0x34]; struct N *n50; };
extern u32 gUnk_02000470;
void sub_08013684(u8 *);
void sub_08013C74(void *, void *);
void sub_08219D38(void *);
u32 sub_08013CA8(u8 *p)
{
    struct N *e = *(struct N **)(p + 0x18);
    struct N *nx;
    while (e != 0) {
        nx = e->n50;
        sub_08013684((u8 *)e + 0xc);
        sub_08013C74(p, e);
        sub_08219D38(e);
        e = nx;
    }
    {
        u32 z = 0;
        *(u32 *)(p + 0x18) = z;
        gUnk_02000470 = z;
    }
    return 0;
}
