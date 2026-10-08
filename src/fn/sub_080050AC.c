#include "global.h"

struct S { u8 t; u8 f1; u16 a; u16 b; u8 g[0xb]; u8 c; u8 h[4]; s16 hp; u8 i[1]; u8 d; u8 j[0x18a]; void *fn; };
void sub_08005338(void);
void sub_080050AC(struct S *p)
{
    u8 t = p->t;
    if (t == 1) {
        if (p->hp <= 0) {
            void *f = sub_08005338;
            p->fn = f;
            p->a = 0;
            p->c = t;
            p->b = 3;
        }
    } else {
        if (p->hp <= 0) {
            p->d = 3;
            p->fn = 0;
        }
    }
}
