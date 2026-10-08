#include "global.h"
struct S { u8 f[0xFE4]; void *fp; u32 a; u32 b[2]; u32 c; u32 d[2]; u8 st; u8 pad[7]; u32 h; u8 q[0x18]; u8 c24; u8 pad2; u16 t; };
void sub_0803386C(u32, u32);
void sub_08033924(u32, u32);
void sub_080337FC(u32);
s32 sub_0822BE78(void);
void sub_08109374(void);
void sub_08110410(void);
void sub_081102EC(void);
static inline void set(struct S *s, void *fp, u8 st) { s->fp = fp; s->c24 = 1; s->st = st; }
void sub_0811024C(struct S *s)
{
    if (s->c24 != 0) {
        sub_0803386C(s->h, s->c);
        sub_08033924(s->h, 4);
        sub_080337FC(s->h);
        s->t = 0;
        s->c24 = 0;
    }
    if (s->t == 0x3c) {
        if (!(u8)sub_0822BE78()) {
            set(s, sub_08110410, 8);
        } else {
            sub_08109374();
            set(s, sub_081102EC, 5);
        }
    } else {
        s->t++;
    }
}
