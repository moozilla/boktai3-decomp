#include "global.h"
struct G { u8 f[0x53]; u8 b; };
struct S { u8 f[0xFF0]; u32 e; u8 p[8]; s32 i; u32 pad; u32 h; };
extern struct G *gUnk_020004B4;
void sub_0803386C(u32, u32);
void sub_08033924(u32, u32);
void sub_08033A38(u32, u32, void *);
void sub_080337FC(u32);
static inline u8 ok(s32 v) { if (v < 0) goto z; if (v <= 1) goto o; z: return 0; o: return 1; }
void sub_0810F990(struct S *s)
{
    s32 i = s->i;
    struct G *g = gUnk_020004B4;
    if (g != 0 && g->b == 1 && ok(i)) {
        u8 *q = (u8 *)g + ((i << 6) + 0x120);
        if (q != 0) {
            sub_0803386C(s->h, s->e);
            sub_08033924(s->h, 0);
            sub_08033A38(s->h, 1, q + 0x10);
            sub_080337FC(s->h);
        }
    }
}
