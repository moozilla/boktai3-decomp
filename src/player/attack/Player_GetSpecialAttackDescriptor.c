#include "global.h"
#include "player/player.h"

extern struct PlayerIndexedAction gUnk_08E60F4C[];

struct PlayerIndexedAction *Player_GetSpecialAttackDescriptor(s32 action)
{
    s32 index = action - 1;

    return &gUnk_08E60F4C[index];
}
