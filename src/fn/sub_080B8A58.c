#include "global.h"
#include "ram.h"
/* Skeleton_SelectRandomBehavior: Selects a different random eight-byte behavior entry and resets its cursor; behavior interpretation is tentative */
#define Skeleton_SelectRandomBehavior sub_080B8A58

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

struct S080B8A58 {
    u8 pad0[2];
    u8 index;
    u8 unk3;
    u8 *data;
    u8 *entry;
};

void sub_080B8A58(u8 *owner)
{
    struct S080B8A58 *p = (struct S080B8A58 *)(owner + 0x448);

    u8 n;
    n = Mod(Random(), *p->data);
    if (n == p->index)
        n = Mod(n + 1, *p->data);
    p->index = n;
    p->unk3 = 0;
    p->entry = Path_GetPointData(p->data) + p->index * 8;
}
