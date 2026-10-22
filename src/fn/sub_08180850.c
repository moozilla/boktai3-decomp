#include "global.h"
struct A {
    u8 pad[0x38];
    u16 f38;
};
u32 sub_08180850(struct A *p)
{
    p->f38 = 1;
}