#include "global.h"

struct Pair080AA024 {
    u32 a, b;
};
struct Entry080AA024 {
    struct Pair080AA024 pos;
    u8 pad8[0xf0];
    struct Pair080AA024 pos2;
    u8 pad100[0x128 - 0x100];
};
struct Owner080AA024 {
    u8 pad0[0x3f0];
    struct Entry080AA024 entries[4];
    u8 pad890[0x8da - 0x890];
    s16 count;
};

void sub_080AA024(struct Owner080AA024 *p, struct Pair080AA024 *pos)
{
    s32 i;
    for (i = 0; i < p->count; i++) {
        p->entries[i].pos = *pos;
        p->entries[i].pos2 = *pos;
    }
}
