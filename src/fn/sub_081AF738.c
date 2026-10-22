#include "global.h"
struct Vec {
    s16 x, y, z;
};
void DamageNumberDisplay_Show(u32, u32, u32, u32, struct Vec *, u32);
static inline void Vec_Set(struct Vec *v, s16 x, s16 y, s16 z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}
void sub_081AF738(u32 a, u32 b, u32 c, u32 d, u32 e)
{
    struct Vec v;
    Vec_Set(&v, 32, 0, 32);
    DamageNumberDisplay_Show(a, b, c, d, &v, e);
}