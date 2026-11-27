#include "objects/moving_platforms/named_object.h"
#include "player/player.h"

void Player_AddAttachmentFlags(u8 *, u32, u32);
void Player_ClearAttachmentFlags(u8 *, u32);

void MovingPlatform_UpdatePlayerFollowFlags(struct NamedObject *object)
{
    if ((u16)(object->state - 3) > 1) {
        Player_AddAttachmentFlags((*(u8 **)gPlayers), 5, object->name);
    } else {
        Player_AddAttachmentFlags((*(u8 **)gPlayers), 1, object->name);
        Player_ClearAttachmentFlags((*(u8 **)gPlayers), 4);
    }
}
