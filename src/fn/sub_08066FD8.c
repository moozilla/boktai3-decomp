#include "global.h"

struct Ent08066FD8 {
    u8 pad0[0x3e];
    s16 state;
    u16 flags;
    s16 id;
    u8 pad44[0xac - 0x44];
};
struct Pool08066FD8 {
    u8 pad0[0x3c];
    struct Ent08066FD8 ents[1];
};
extern struct Pool08066FD8 *gUnk_02000128;

void sub_08066FD8(s32 index)
{
    struct Pool08066FD8 *g = gUnk_02000128;
    if (index >= 0) {
        struct Ent08066FD8 *e = &g->ents[index];
        if (e->id >= 0 && e->state != 2)
            e->flags |= 4;
    }
}
