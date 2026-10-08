#include "global.h"
s32 Div(s32, s32);
struct S { u8 f[0x1a]; u8 o; u8 p[0x1d]; s32 v; s32 a; s32 b; s32 c; s32 d; };
struct T { s32 x; s32 a; s32 b; s32 c; s32 d; };
void sub_08002EE4(struct S *s)
{
    struct T *t = (struct T *)&s->v;
    s32 r;
    s32 c = t->c;
    if (c == 0) {
        r = t->a;
    } else {
        s32 d = t->d;
        if (d == 0 || c >= d) {
            s->v = t->b;
            goto end;
        }
        r = Div(t->a * (d - c) + c * t->b, d);
    }
    s->v = r;
    t->c++;
end:
    s->o = t->x;
}
