#include "global.h"
struct BF { u32 a : 16; u32 b : 16; u32 c : 16; };
struct R { u8 pad[0xc]; u16 a; u16 b; u16 c; };
struct S { u8 pad[8]; s16 id; s16 a; s16 b; s16 c; s16 d; };
struct R *sub_08225984(s32);
void sub_08018C48(struct BF *, s32, s32, s32, s32);
void sub_08020FE4(u32 x, u32 y, struct S *s)
{
    struct BF bf;
    struct R *r = sub_08225984(s->id);
    if (r != 0) {
        bf.a = r->a;
        bf.b = r->b;
        bf.c = r->c;
        sub_08018C48(&bf, s->a, s->b, s->c, s->d);
    }
}
