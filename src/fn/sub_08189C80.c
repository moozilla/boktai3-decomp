#include "global.h"
struct S { u8 f[0xB32]; u16 fl; };
void sub_081885BC(void *);
void sub_0818911C(void *);
void sub_08188B7C(void *);
void sub_081896D0(void *);
void sub_08189C80(struct S *p)
{
    if ((p->fl & 0x200) == 0)
        sub_08188B7C(p);
    else
        sub_081896D0(p);
}
