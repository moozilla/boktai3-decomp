#include "global.h"

struct S { u8 f[0x1AA4]; u8 a; u8 g[3]; u8 m; };

void sub_0805EA00(u8 *);
void sub_0806147C(struct S *);
void sub_080607B0(struct S *);
void sub_08060F4C(struct S *);
void sub_08060C54(struct S *);
void sub_0806130C(struct S *, void (*)(void));
void sub_08062414(void);

void sub_080619D4(struct S *p)
{
    p->a = 0x80;
    sub_0805EA00((u8 *)p + 0x1BE4);
    sub_0806147C(p);
    switch (p->m) {
    case 0:
        sub_080607B0(p);
        break;
    case 1:
        sub_08060F4C(p);
        break;
    case 2:
        sub_08060C54(p);
        break;
    }
    sub_0806130C(p, sub_08062414);
}
