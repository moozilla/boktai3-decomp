#include "global.h"
u8 DisplaySlots_Reserve(u32, u32);
void CharacterModel_SetSecondaryAppearance(u8 *p, u32 x, u32 y)
{
    *(u8 *)(p + 0x251) = DisplaySlots_Reserve(x, y);
}
