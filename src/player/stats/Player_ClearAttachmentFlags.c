#include "player/player.h"

void Player_ClearAttachmentFlags(struct Player *p, u32 a)
{
    p->tetherStatusFlags &= ~a;
    p->tetherStatusTag = 0;
}
