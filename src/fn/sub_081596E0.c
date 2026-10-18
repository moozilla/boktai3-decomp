#include "global.h"
void Player_IncreaseLifeTowardLimit(u8 *, u32);
static inline u32 tst(u32 *a, u32 m)
{
    return *a & m;
}
void Player_RestoreLifeWithModifier(u8 *p, u32 n)
{
    if (p != 0 && tst((u32 *)(p + 0x444), 0x1000))
        Player_IncreaseLifeTowardLimit(p, n << 1);
    else
        Player_IncreaseLifeTowardLimit(p, n);
}
