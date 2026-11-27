#include "global.h"
#include "recovered/contact_reaction_vector_layout.h"
/* DamageNumberDisplay_ShowDefaultSize: Shows a damage number using a fixed size vector of 32 by zero by 32 */

void DamageNumberDisplay_Show(u32, u32, u32, u32, struct ContactReactionVector *, u32);

static inline void SetVec(struct ContactReactionVector *v, u16 x, u16 y, u16 z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}

void DamageNumberDisplay_ShowDefaultSize(u32 a, u32 b, u32 c, u32 d, u32 e)
{
    struct ContactReactionVector size;
    SetVec(&size, 0x20, 0, 0x20);
    DamageNumberDisplay_Show(a, b, c, d, &size, e);
}
