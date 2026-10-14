#include "global.h"

struct Vec0819AE10 {
    u16 x, y, z, w;
};
void DamageNumberDisplay_Show(u32, u32, u32, u32, struct Vec0819AE10 *, u32);

static inline void SetVec(struct Vec0819AE10 *v, u16 x, u16 y, u16 z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}

void sub_0819AE10(u32 a, u32 b, u32 c, u32 d, u32 e)
{
    struct Vec0819AE10 size;
    SetVec(&size, 0x20, 0, 0x20);
    DamageNumberDisplay_Show(a, b, c, d, &size, e);
}
