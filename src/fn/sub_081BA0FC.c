#include "global.h"

struct Q { u8 f[0x52a]; u16 h52a; u8 g[0x2c]; void *p558; };
void sub_081BB150(void *);
void sub_081B984C(void *, void (*)(void));
void sub_081BA1D4(void);

void sub_081BA0FC(struct Q *p)
{
    if (p->h52a == 0)
        sub_081BB150(p->p558);
    sub_081B984C(p, sub_081BA1D4);
}
