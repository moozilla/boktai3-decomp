#include "global.h"
#include "player/player.h"

u8 Player_GetSmoothedDirection(struct Player *);
void Player_UpdateFacingSide(struct Player *);

s32 Player_UpdateAttackFacing(struct Player *p)
{
    s32 r = 0;

    if ((s16)p->directionHistory[0] != -1 && p->cameraOffsetActive == 0) {
        p->facing = Player_GetSmoothedDirection(p);
        r = TRUE;
    }
    Player_UpdateFacingSide(p);
    return r;
}
