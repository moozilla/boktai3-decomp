#include "global.h"

struct S { u8 pad[0x30]; void (*f30)(void); s16 f34; s16 f36; };
void sub_081C4C18(struct S *);
void sub_081C4EA0(void);

void sub_081C4C70(struct S *p)
{
    if (p->f36 < 0) {
        sub_081C4C18(p);
    } else {
        p->f30 = sub_081C4EA0;
        p->f34 = 0;
    }
}
