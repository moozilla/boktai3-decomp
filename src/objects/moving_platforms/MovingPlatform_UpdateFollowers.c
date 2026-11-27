#include "global.h"
#include "player/player.h"

void MovingPlatform_UpdatePlayerAttachment(u8 *);
void MovingPlatform_UpdateTetherAttachment(u8 *);
s32 Player_TestAttachmentFlags(u8 *, u32);
void MovingPlatform_MoveAttachedPlayer(u8 *);
s32 TetherActor_TestStatusFlags(u32);
void MovingPlatform_MoveAttachedTether(u8 *);
void Memory_Clear(u8 *, u32);

void MovingPlatform_UpdateFollowers(u8 *s)
{
    if ((*(u8 **)gPlayers) != 0) {
        MovingPlatform_UpdatePlayerAttachment(s);
        MovingPlatform_UpdateTetherAttachment(s);
        if (Player_TestAttachmentFlags((*(u8 **)gPlayers), 1))
            MovingPlatform_MoveAttachedPlayer(s);
        if (TetherActor_TestStatusFlags(1))
            MovingPlatform_MoveAttachedTether(s);
        Memory_Clear(s + 0x58, 8);
    }
}
