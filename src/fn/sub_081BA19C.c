#include "global.h"

struct Q { u8 f[0x520]; s16 h520; u8 g[0x0]; u8 pad[0]; };
struct Q2 { u8 f[0x522]; u8 sh; };
void sub_081B9D54(void *);
void sub_081B984C(void *, void *);

void sub_081BA19C(struct Q *p)
{
    sub_081B9D54(p);
    p->h520++;
    if (p->h520 > (1 << ((struct Q2 *)p)->sh))
        sub_081B984C(p, 0);
}
