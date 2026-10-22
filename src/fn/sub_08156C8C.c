#include "global.h"
void Player_RestoreCameraFollow(u8 *);

void sub_08156C8C(u8 *player)
{
    if (*(s16 *)(player + 0x4b6) != 0)
        Player_RestoreCameraFollow(player);
}
