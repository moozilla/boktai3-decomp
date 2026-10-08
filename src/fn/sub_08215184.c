#include "global.h"

struct Ent { u16 id; u8 pad[6]; };
struct Hdr { u8 pad[6]; u16 n; u8 pad2[4]; struct Ent ents[1]; };
extern struct Hdr *gUnk_030042EC;

struct Ent *sub_08215184(u16 id)
{
    struct Hdr *h = gUnk_030042EC;
    if (h == 0)
        return 0;
    {
        struct Ent *e = h->ents;
        s32 i = 0;
        for (; i < h->n; i++, e++) {
            if (e->id == id)
                return e;
        }
    }
    return 0;
}
