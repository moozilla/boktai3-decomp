#include "global.h"

struct E { u8 f[4]; u8 a; u8 g[0x6b]; u8 b; u8 h[0x77]; };
struct A { u8 f[0x64]; struct E e[16]; };
extern void *gUnk_02000244;
void sub_082195E0(void *);
void sub_08013B74(void *);

void sub_081A4DB4(struct A *p)
{
    s32 i;
    struct E *e = p->e;
    for (i = 15; i >= 0; i--) {
        if (e->a != 0)
            sub_082195E0(e);
        if (e->b != 0)
            sub_08013B74(&e->b);
        e++;
    }
    gUnk_02000244 = 0;
}
