#include "global.h"

struct S { u8 f[0x1AA4]; u8 a; u8 g[3]; u8 m; };

void sub_08060660(struct S *);
void sub_08060EA4(struct S *);
void sub_08060B94(struct S *);
void sub_0806130C(struct S *, void (*)(void));
void sub_08062634(void);

void sub_08061A40(struct S *p)
{
    p->a = 0x80;
    switch (p->m) {
    case 0:
        sub_08060660(p);
        break;
    case 1:
        sub_08060EA4(p);
        break;
    case 2:
        sub_08060B94(p);
        break;
    }
    sub_0806130C(p, sub_08062634);
}
