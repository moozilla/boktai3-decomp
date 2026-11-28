#include "global.h"
#include "player/player.h"

void Player_RefreshEquipment(struct Player *p, s32 q);

void sub_08140244(struct Player *p)
{
    Player_RefreshEquipment(p, 0);
}
