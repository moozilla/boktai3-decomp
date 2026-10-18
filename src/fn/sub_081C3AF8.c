#include "global.h"

struct S {
    u8 pad[0xf8];
    u32 f8;
};

void RegistryF3E4_UpdateVisibleIdle(struct S *p)
{
    p->f8++;
}
