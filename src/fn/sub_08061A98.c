#include "global.h"

struct S { u8 f[0x1AA4]; u8 a; u8 g[3]; u8 m; };

void sub_0806055C(struct S *);
void sub_08060DB4(struct S *);
void sub_08060A70(struct S *);
void sub_0806130C(struct S *, void (*)(void));
void sub_08062324(void);

void sub_08061A98(struct S *p)
{
    p->a = 0x80;
    switch (p->m) {
    case 0:
        sub_0806055C(p);
        break;
    case 1:
        sub_08060DB4(p);
        break;
    case 2:
        sub_08060A70(p);
        break;
    }
    sub_0806130C(p, sub_08062324);
}
