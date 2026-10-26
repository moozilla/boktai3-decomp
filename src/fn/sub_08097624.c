#include "global.h"

extern u32 gRandomIndex;
extern u16 gUnk_0203B400[];
s32 Mod(s32, s32);
u8 *Path_GetPointData(u8 *);

static inline u32 Random(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(u16 *)(offset + (u32)gUnk_0203B400);
}

struct S08097624 {
    u8 pad0[2];
    u8 index;
    u8 unk3;
    u8 *data;
    u8 *entry;
};

u32 Ghost_SelectRandomRouteEntry(struct S08097624 *p)
{
    struct S08097624 *q = p;
    u8 n;
    if (*p->data <= 1)
        return 0;
    n = Mod(Random(), *p->data);
    if (n == p->index)
        n = Mod(n + 1, *p->data);
    q->index = n;
    q->unk3 = 0;
    q->entry = Path_GetPointData(q->data) + q->index * 8;
    return 1;
}
