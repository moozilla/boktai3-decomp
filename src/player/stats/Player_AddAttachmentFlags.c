#include "player/player.h"

void Player_AddAttachmentFlags(struct Player *p, u32 a, u16 b)
{
    p->tetherStatusFlags |= a;
    p->tetherStatusTag = b;
}
