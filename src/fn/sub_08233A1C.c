#include "global.h"
struct Entry33A { u8 pad[15]; u8 a, b; u8 gap[23]; u8 c, d; u8 tail[2]; };
struct State33A { u8 pad[0xa0]; u32 handle; struct Entry33A entries[8]; };
u32 sub_08215184(u32);
void sub_08217DE0(struct Entry33A *, u32, s32);
void sub_08217EC4(struct Entry33A *, s32, s32);
void sub_08217EEC(struct Entry33A *, u32, s32);
void sub_08217ECC(struct Entry33A *, s32);
void sub_08233A1C(struct State33A *p)
{
    s32 i;
    p->handle = sub_08215184(0x1c1e);
    for (i = 0; i < 8; i++) {
        struct Entry33A *e = &p->entries[i];
        sub_08217DE0(e, p->handle, 1);
        sub_08217EC4(e, -4, -4);
        sub_08217EEC(e, p->handle, 8);
        sub_08217ECC(e, 1);
        e->a = 2;
        e->b = 236;
        p->entries[i].c = 0;
        p->entries[i].d = 0;
    }
}
