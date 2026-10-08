#include "global.h"
struct T { u16 a, b, c, d; };
struct V { u16 a, b, c; };
extern const struct T gUnk_0824F1DC[];
void sub_08121D64(struct V *, s32, s32, s32, s32);
void sub_0822B3BC(s32);
void sub_0818F3D8(struct V *p)
{
    s32 i = 0;
    struct V v;
    struct V *q = &v;
    do {
        const struct T *t = &gUnk_0824F1DC[i];
        q->a = p->a + t->a;
        q->b = p->b + t->b;
        q->c = p->c + t->c;
        sub_08121D64(&v, 1, 2, 1, 0x154);
        i++;
    } while (i <= 6);
}
