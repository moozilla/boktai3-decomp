#include "global.h"

struct S { u8 f[0x20]; void *p20; void *p24; };
extern u32 gUnk_0200046C;
void sub_08214514(void *);
void sub_0800CC08(u32, void *);
void sub_0800CC34(void *);
void sub_0821FE6C(void *);
void sub_08219D38(void *);

u32 sub_0800CC50(struct S *s)
{
    sub_08214514((u8 *)s + 0x44);
    if (gUnk_0200046C != 0)
        sub_0800CC08(gUnk_0200046C, s);
    sub_0800CC34(s);
    if (s->p20 != 0) {
        sub_0821FE6C(s->p20);
        sub_08219D38(s->p20);
    }
    if (s->p24 != 0)
        sub_08219D38(s->p24);
    sub_08219D38(s);
}
