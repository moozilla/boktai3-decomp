#include "global.h"
struct S { u8 f[0x864]; u32 c; u8 on; u8 p[3]; void (*cb)(void); };
void sub_08117948(void);
void sub_08117294(struct S *s)
{
    void (*f)(void) = sub_08117948;
    u8 *p = &s->on;
    u32 z = 0;
    *p = 1;
    s->cb = f;
    s->c = z;
}
