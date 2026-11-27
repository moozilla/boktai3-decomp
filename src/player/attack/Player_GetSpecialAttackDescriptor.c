#include "global.h"

extern u8 gUnk_08E60F4C[];

u8 *Player_GetSpecialAttackDescriptor(s32 idx)
{
    return gUnk_08E60F4C + ((idx - 1) << 2);
}
