#define ActorStats_ApplyPendingDamage  sub_0811D0C4
#define ModifierStats_ApplyKinds4And14 sub_0811E9F0
#define PaletteFlash_SelectPalette     sub_0811C99C
#include "global.h"
#include "actor_stats.h"
void Character_SetLowResourceColorEffect(void *, void *);
void sub_0811C99C(void *, u32);
s32 sub_0811E9F0(void *, s32);
static inline u8 TestDamage(struct ActorStats *p)
{
    u32 mask = 1;
    if (p->damageFlags & mask)
        return 1;
    return 0;
}
static inline u32 ClearFlag(u8 *flags, u32 mask)
{
    u32 value = ~mask;
    value &= *flags;
    return value;
}
static inline u32 ReadFlags(u8 *flags)
{
    return *flags;
}
static inline void SetFlag(u8 *flags, u32 mask)
{
    u32 value = mask;
    value |= ReadFlags(flags);
    *flags = value;
}
void sub_0811D0C4(struct ActorStats *p)
{
    if (TestDamage(p)) {
        u32 cleared = ClearFlag(&p->damageFlags, 1);
        u32 zero = 0;
        p->damageFlags = cleared;
        sub_0811C99C(&p->pendingDamage, 1);
        p->amount -= sub_0811E9F0(p, p->pendingDamage);
        p->damageTimer = zero;
        p->pendingDamage = zero;
        Character_SetLowResourceColorEffect(p->state44, p->state48);
        if (p->amount <= 0)
            SetFlag(&p->damageFlags, 2);
    }
}
