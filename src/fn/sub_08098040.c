#include "global.h"

struct Ent08098040 {
    u32 value;
    u8 pad4[0x60 - 4];
};
struct Group08098040 {
    u8 pad0[0x40c];
    struct Ent08098040 ents[4];
};

void sub_08098040(u8 *p, u32 value)
{
    struct Group08098040 *g = *(struct Group08098040 **)(p + 0x3d0);
    s32 i;
    for (i = 3; i >= 0; i--)
        g->ents[i].value = value;
}
