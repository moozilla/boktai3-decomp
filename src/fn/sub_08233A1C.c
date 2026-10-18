#include "global.h"
struct Entry33A {
    u8 pad[15];
    u8 a, b;
    u8 gap[23];
    u8 c, d;
    u8 tail[2];
};
struct State33A {
    u8 pad[0xa0];
    u32 handle;
    struct Entry33A entries[8];
};
u32 AnimData_FindEntry(u32);
void sub_08217DE0(struct Entry33A *, u32, s32);
void Effect_SetOrigin(struct Entry33A *, s32, s32);
void sub_08217EEC(struct Entry33A *, u32, s32);
void Effect_SetPalette(struct Entry33A *, s32);
void ChargedBurst_InitParticles(struct State33A *p)
{
    s32 i;
    p->handle = AnimData_FindEntry(0x1c1e);
    for (i = 0; i < 8; i++) {
        struct Entry33A *e = &p->entries[i];
        sub_08217DE0(e, p->handle, 1);
        Effect_SetOrigin(e, -4, -4);
        sub_08217EEC(e, p->handle, 8);
        Effect_SetPalette(e, 1);
        e->a = 2;
        e->b = 236;
        p->entries[i].c = 0;
        p->entries[i].d = 0;
    }
}
