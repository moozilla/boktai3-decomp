#include "global.h"
struct S { u8 f[0xFE4]; void *fp; u8 p[0x18]; u8 st; u8 q[0x23]; u8 c; };
u8 sub_0822BEC0(void);
void sub_08110378(void);
void sub_08110410(void);
static inline void set(struct S *s, void *fp, u8 st) { s->fp = fp; s->c = 1; s->st = st; }
void sub_08110334(struct S *s)
{
    if (sub_0822BEC0())
        set(s, sub_08110378, 7);
    else
        set(s, sub_08110410, 8);
}
