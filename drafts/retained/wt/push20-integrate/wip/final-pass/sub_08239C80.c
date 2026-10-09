#include "global.h"
struct Entry { u16 a, b; };
struct State { u8 pad[0x18]; u32 value; u8 gap[0xa94]; struct Entry entries[4]; };
void sub_08239C80(struct State *p)
{
    u32 v = p->value + 47;
    p->entries[0].a = v;
    p->entries[0].b = 366;
    p->entries[1].a = 45;
    p->entries[1].b = 368;
    p->entries[2].a = 369;
    p->entries[2].b = 44;
    p->entries[3].a = 46;
    p->entries[3].b = 43;
}
