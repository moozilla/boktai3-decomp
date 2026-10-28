#include "global.h"
#include "ram.h"

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

struct S0809CB58 {
    u8 pad0[2];
    u8 index;
    u8 unk3;
    u8 *data;
    u8 *entry;
};

void Lun_SelectRandomRouteEntry(struct S0809CB58 *p)
{
    struct S0809CB58 *q = p;
    u8 n;
    n = Mod(Random(), *p->data);
    if (n == p->index)
        n = Mod(n + 1, *p->data);
    q->index = n;
    q->unk3 = 0;
    q->entry = Path_GetPointData(q->data) + q->index * 8;
}
