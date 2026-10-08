#include "global.h"

struct Ent { u16 id; u8 pad[6]; };
struct Hdr { u16 n; u8 pad[0xA]; struct Ent *ents; };

struct Ent *sub_08215C8C(struct Hdr *h, u16 id)
{
    struct Ent *e = h->ents;
    s32 i = 0;
    for (; i < h->n; e++, i++) {
        if (e->id == id)
            return e;
    }
    return 0;
}
