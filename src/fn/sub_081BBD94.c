#include "global.h"
struct P {
    u8 f[0x730];
    u32 a;
};
void sub_0824923C(void *, u32);
s32 SolarBank_OnUpdate(struct P *p)
{
    sub_0824923C(p, p->a);
    return 0;
}
