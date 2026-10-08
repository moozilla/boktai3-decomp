#include "global.h"

struct S0813B154 { u8 filler[0x418]; u8 v; };
void sub_0822B358(u32);

void sub_0813B154(struct S0813B154 *p)
{
    if (p->v != 6) {
        sub_0822B358(0xd8);
    } else {
        sub_0822B358(0x239);
        sub_0822B358(0x202);
        sub_0822B358(0x366);
    }
}
