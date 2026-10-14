// COMPILER: old_agbcc
#include "global.h"

struct Pos080ACCD4 {
    u32 a, b;
};
struct Entry080ACCD4 {
    u8 pad0[0x1c];
    struct Pos080ACCD4 pos;
    u8 pad24[0x128 - 0x24];
};
struct Group080ACCD4 {
    u8 pad0[0x3d4];
    struct Entry080ACCD4 entries[4];
};

void sub_080ACCD4(struct Group080ACCD4 *g, s32 i, struct Pos080ACCD4 *pos)
{
    g->entries[i].pos = *pos;
}
