#include "global.h"
#include "player_slot_status.h"
void Player_IncreaseLifeTowardLimit(u8 *, u32);
void Player_RestoreLifeWithModifier(u8 *p, u32 n)
{
    if (p != 0 && Player_ReadStatusBits((u32 *)(p + 0x444), 0x1000))
        Player_IncreaseLifeTowardLimit(p, n << 1);
    else
        Player_IncreaseLifeTowardLimit(p, n);
}
