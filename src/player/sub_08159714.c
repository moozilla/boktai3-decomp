#include "engine/shared/global.h"
#include "engine/shared/player_slot_status.h"
void Player_AddEnergy(u8 *, u32);
void Player_RestoreEnergyWithModifier(u8 *p, u32 n)
{
    if (p != 0 && Player_ReadStatusBits((u32 *)(p + 0x444), 0x1000))
        Player_AddEnergy(p, n << 1);
    else
        Player_AddEnergy(p, n);
}
