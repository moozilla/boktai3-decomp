#include "global.h"
struct Entry080A985C {
    u8 pad0[0x1c];
    u32 pos[2];
    u8 pad24[0x105 - 0x24];
    u8 flags;
    u8 pad106[0x128 - 0x106];
};
struct Group080A985C {
    u8 pad0[0x3d4];
    struct Entry080A985C entries[4];
    u8 pad874[0x888 - 0x874];
    u8 obj[0x8b4 - 0x888];
    struct Entry080A985C *target;
    u8 pad8b8[0x8d0 - 0x8b8];
    u16 state;
    u8 pad8d2[0x8da - 0x8d2];
    s16 count;
};
void sub_08225AB4(void *, s32, void *, u32, u32, void *);
void sub_08003C00(void *);
inline static u8 TestMask(u8 *flags, char mask)
{
    if ((*flags) & mask)
        return 1;
    return 0;
}

void sub_080A985C(u8 *p)
{
    struct Group080A985C *g = *(struct Group080A985C **)(p + 0x3d0);
    s32 found = 0xff;
    s32 i;
    for (i = 0; i < g->count; i++) {
        if (TestMask(&g->entries[i].flags, 4)) {
            found = i;
            break;
        }
    }

    if (found != 0xff) {
        void *obj = g->obj;
        struct Entry080A985C *e; /* unused, but its declaration affects register allocation */
        u16 m;
        sub_08225AB4(obj, *(s32 *)(p + 0x50) + 1, g->entries[found].pos, 0, 7, g);
        g->target = &g->entries[found];
        sub_08003C00(obj);
        m = 1;
        g->state |= m;
    }
}
