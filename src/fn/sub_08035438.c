#include "global.h"

struct S {
    u8 f[0x21];
    u8 a;
};
u32 sub_0803536C(void);
void rfu_UNI_readySendData(u32);
void sub_08223580(u32);
void sub_08035438(struct S *p)
{
    if (sub_0803536C())
        rfu_UNI_readySendData(p->a);
    sub_08223580(1);
}
