#include "global.h"
struct S { u8 f[0x588]; u16 h; };
struct T { u8 f[0x522]; u16 h; };
extern struct T *gUnk_02000580;
void sub_081C1A98(struct S *p)
{
    if (gUnk_02000580->h > 3) p->h = p->h + 1;
    else p->h = 0;
}
