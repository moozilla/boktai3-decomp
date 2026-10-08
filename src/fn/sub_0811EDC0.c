#include "global.h"
void sub_0811ECF0(void);
struct S { void (*fn)(void); u32 a; u16 h; u16 b; u8 pad; u8 c; u8 d; u8 f; };
void sub_0811EDC0(struct S *p, u32 a, u16 b, u8 c)
{
    u8 z = 0;
    p->f = z;
    p->h = z;
    p->b = b;
    p->c = c;
    p->a = a;
    p->fn = sub_0811ECF0;
    p->d = 1;
}
