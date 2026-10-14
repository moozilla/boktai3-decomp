#include "global.h"

struct S {
    u8 f[0x184];
    u8 a;
};
void DisplayAnimation_Destroy(void *);
void sub_0802D43C(struct S *p)
{
    DisplayAnimation_Destroy(&p->a);
}
