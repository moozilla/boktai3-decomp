#include "global.h"
struct Elem {
    u32 flags;
    u8 pad4[11];
    u8 attr;
    u8 pad10[0x18];
    u16 age, active;
    u8 pad2C[8];
    u32 data;
    void (*update)(struct Elem *);
};
struct Owner {
    u8 pad[0x168];
    struct Elem elems[8];
};
u32 AnimData_FindEntry(u32);
void sub_08217DE0(struct Elem *, u32, s32);
void Effect_SetOrigin(struct Elem *, s32, s32);
void SpriteDrawable_SelectTileFrame(struct Elem *, u32, s32);
void Effect_SetPalette(struct Elem *, s32);
void HealthBurstEffect_InitParticlePool(struct Owner *p)
{
    u32 resource = AnimData_FindEntry(0x1c1e);
    u32 zero = 0;
    struct Elem *e = (struct Elem *)((u8 *)p + 0x168);
    s32 i;
    for (i = 0; i < 8; e++, i++) {
        p->elems[i].data = resource;
        sub_08217DE0(e, resource, 17);
        Effect_SetOrigin(e, -4, -4);
        SpriteDrawable_SelectTileFrame(e, p->elems[i].data, 2);
        Effect_SetPalette(e, 1);
        e->attr = zero;
        e->age = zero;
        e->active = zero;
    }
}
