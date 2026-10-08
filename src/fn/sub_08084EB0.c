#include "global.h"
void sub_08214514(void *);
void sub_082195E0(void *);
void sub_08219D38(void *);
void sub_08020D08(void *);
void sub_08013684(void *);
void sub_08003C10(void *);
void sub_08225938(void *);
void sub_0821FE6C(void *);
void sub_0806FBC8(void *);

static inline u8 tst(u16 *a, u32 m) { if (*a & m) return 1; return 0; }
struct S {
    u8 f0[4]; u8 *p4; u8 b8; u8 f1[0x2b7];
    u16 h2c0;
};
u32 sub_08084EB0(struct S *s) {
    u8 *p = s->p4;
    u8 *q;
    u32 m;
    u32 v;
    if (s->b8 == 0) sub_08214514(p);
    else sub_082195E0(p + 0x20);
    sub_08219D38(p);
    m = 0x100;
    if (tst((u16 *)((u8 *)s + 0x2c0), m)) sub_08020D08((u8 *)s + 0xc);
    {
        u8 *r = (u8 *)s + 0x320;
        if (*r == 1) sub_08013684(r);
    }
    m = 0x2000000;
    v = *(u32 *)((u8 *)s + 0x2d0) & m;
    q = (u8 *)s + 0x50;
    if (v == 0) sub_08003C10(q);

    sub_08225938(q);
    sub_0821FE6C((u8 *)s + 0x134);
    sub_0806FBC8(s);
    return 0;
}
