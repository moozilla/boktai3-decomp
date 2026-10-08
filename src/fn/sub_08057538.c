#include "global.h"

struct S { u8 f[0x18]; u16 v; };
extern struct S *gUnk_0200049C;
void sub_08178404(void);
void sub_0821A0C0(struct S *);

void sub_08057538(void)
{
    if (gUnk_0200049C) {
        struct S *p;
        sub_08178404();
        p = gUnk_0200049C;
        p->v = 0;
        sub_0821A0C0(p);
    }
}
