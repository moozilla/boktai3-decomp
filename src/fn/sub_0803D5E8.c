#include "global.h"

struct S {
    u8 pad[0x18];
    u32 count;
    u8 pad2[4];
    u8 *items;
};

u8 *sub_0803D5E8(struct S *s, u32 *idx)
{
    u32 i;
    u8 *e = s->items;
    for (i = 0; i < s->count;) {
        if (e[0x24] == 0) {
            *idx = i;
            return e;
        }
        i++;
        e += 0x17c;
    }
    return 0;
}
