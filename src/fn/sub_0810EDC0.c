#include "global.h"
struct H { u8 filler[0x14E4]; u32 a; u8 pad[8]; u32 c; u32 d; };
struct G2 { u8 f[0x53]; u8 b; u8 p[0xcc]; u8 a[0x10]; u8 c[0x30]; u8 e[0x10]; u8 g[1]; };
extern struct G2 *gUnk_020004B4;
void sub_0803386C(u32, u32);
void sub_08033924(u32, u32);
void sub_08033A38(u32, u32, void *);
void sub_080337FC(u32);
void sub_0810EDC0(struct H *h)
{
    struct G2 *g = gUnk_020004B4;
    if (g != 0 && g->b == 1 && g->a != 0) {
        sub_0803386C(h->c, h->a);
        sub_08033924(h->c, 0);
        sub_08033A38(h->c, 1, g->c);
        sub_080337FC(h->c);
        g = gUnk_020004B4;
        if (g != 0 && g->b == 1 && g->e != 0) {
            sub_0803386C(h->d, h->a);
            sub_08033924(h->d, 1);
            sub_08033A38(h->d, 2, g->g);
            sub_080337FC(h->d);
        }
    }
}
