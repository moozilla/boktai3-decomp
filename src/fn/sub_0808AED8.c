#include "global.h"
#include "engine_globals.h"
#include "ram.h"
/* Ghoul_SelectRandomBehaviorEntry: Chooses a random entry different from the current index and resets its progress byte */
#define Ghoul_SelectRandomBehaviorEntry sub_0808AED8

s32 Mod(s32, s32);
u8 *Path_GetPointData(u8 *);

static inline u32 Random(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(u16 *)(offset + (u32)gRandomTable);
}

struct S0808AED8 {
    u8 pad0[2];
    u8 index;
    u8 unk3;
    u8 *data;
    u8 *entry;
};

void sub_0808AED8(u8 *owner)
{
    struct S0808AED8 *p = (struct S0808AED8 *)(owner + 0x448);
    struct S0808AED8 *q = p;
    u8 n;
    n = Mod(Random(), *p->data);
    if (n == p->index)
        n = Mod(n + 1, *p->data);
    q->index = n;
    q->unk3 = 0;
    q->entry = Path_GetPointData(q->data) + q->index * 8;
}
