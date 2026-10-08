#include "global.h"

struct S { u8 f[0x32]; u8 a; };
void sub_082236BC(void);
u32 sub_080352CC(struct S *p)
{
    if (p->a) {
        p->a = 0;
        sub_082236BC();
    }
    return 0;
}
