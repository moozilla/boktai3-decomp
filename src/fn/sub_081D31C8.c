#include "global.h"

struct S {
    u8 pad[0x3c];
    u32 f3c;
};

void FrostEmitter_StopEmission(struct S *p)
{
    if (p)
        p->f3c = 0;
}
