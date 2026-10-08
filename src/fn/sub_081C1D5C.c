#include "global.h"
struct S { u8 f[0x57a]; u16 h; };
void sub_081C1820(void *);
void sub_081C1D2C(void *);
void sub_081C17D4(void *, void (*)(void));
void sub_081C1FC0(void);
void sub_081C1D5C(struct S *p)
{
    sub_081C1820(p);
    sub_081C1D2C(p);
    p->h = 0x20;
    sub_081C17D4(p, sub_081C1FC0);
}
