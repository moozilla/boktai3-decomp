#include "global.h"

struct S {
    u8 f[0xee];
    u16 a;
};
u32 WirelessController_SetLocalInput(struct S *p, u16 v)
{
    p->a = v;
}
